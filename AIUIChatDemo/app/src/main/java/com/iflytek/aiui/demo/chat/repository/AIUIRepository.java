package com.iflytek.aiui.demo.chat.repository;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.MutableLiveData;
import android.arch.lifecycle.Observer;
import android.content.Context;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.util.Base64;

import com.iflytek.aiui.AIUIAgent;
import com.iflytek.aiui.AIUIConstant;
import com.iflytek.aiui.AIUIEvent;
import com.iflytek.aiui.AIUIListener;
import com.iflytek.aiui.AIUIMessage;
import com.iflytek.aiui.demo.chat.common.Constant;
import com.iflytek.aiui.demo.chat.db.MessageDao;
import com.iflytek.aiui.demo.chat.model.Message;
import com.iflytek.aiui.demo.chat.model.Settings;
import com.iflytek.aiui.demo.chat.model.data.DynamicEntityData;
import com.iflytek.aiui.demo.chat.model.data.SpeakableSyncData;
import com.iflytek.aiui.demo.chat.ui.common.SingleLiveEvent;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import io.reactivex.Completable;
import io.reactivex.functions.Action;
import io.reactivex.schedulers.Schedulers;

import static com.iflytek.aiui.demo.chat.model.Message.FromType.AIUI;
import static com.iflytek.aiui.demo.chat.model.Message.FromType.USER;
import static com.iflytek.aiui.demo.chat.model.Message.MsgType.TEXT;
import static com.iflytek.aiui.demo.chat.model.Message.MsgType.Voice;

/**
 * Created by PR on 2017/7/26.
 */

public class AIUIRepository{
    private Context mContext;
    private AIUIAgent mAgent;

    private LiveData<List<Message>> mInteractMsgFlowable;


    private Message mAppendVoiceMsg = null;

    private MessageDao mMessageDao;

    private int mCurrentState = AIUIConstant.STATE_IDLE;

    private long mAudioStart = System.currentTimeMillis();
    private Settings mCurrentSettings;
    private JSONObject mLastConfig;
    private SettingsRepo mSettingsRepo;
    private MutableLiveData<String> mUID = new MutableLiveData<>();
    private MutableLiveData<AIUIEvent> mVADEvent = new MutableLiveData<>();
    private MutableLiveData<AIUIEvent> mStateEvent = new SingleLiveEvent<>();
    private final AIUIListener mAIUIListener;


    public AIUIRepository(Context context, JSONObject config, MessageDao dao, SettingsRepo settings){
        mLastConfig = config;
        mContext = context;
        mMessageDao = dao;
        mSettingsRepo = settings;
        mInteractMsgFlowable = mMessageDao.getAllMessage();

        //AIUI事件回调监听器
        mAIUIListener = new AIUIListener() {
            @Override
            public void onEvent(AIUIEvent aiuiEvent) {
                switch (aiuiEvent.eventType) {
                    case AIUIConstant.EVENT_WAKEUP:{
                        mStateEvent.postValue(aiuiEvent);
                        if(mCurrentSettings.wakeup) {
                            beginAudio();
                        }
                    }
                    break;

                    case AIUIConstant.EVENT_SLEEP: {
                        mStateEvent.postValue(aiuiEvent);
                    }
                    break;

                    case AIUIConstant.EVENT_STATE: {
                        //oneshot不抛出SLEEP消息
                        if(mCurrentSettings.wakeup && mCurrentState == AIUIConstant.STATE_WORKING && aiuiEvent.arg1 == AIUIConstant.STATE_READY) {
                            endAudio();
                            //READY停止录音机，重新开始录音
                            startRecordAudio();
                        }
                        mCurrentState = aiuiEvent.arg1;
                    }
                    break;

                    case AIUIConstant.EVENT_RESULT: {
                        processResult(aiuiEvent);
                    }
                    break;

                    case AIUIConstant.EVENT_ERROR: {
                        Map<String, String> semantic = new HashMap<>();
                        semantic.put("errorInfo",aiuiEvent.info);
                        if (aiuiEvent.arg1 == 10120) {
                            addMessageToDB(new Message(AIUI, TEXT,
                                    fakeSemanticResult(0, "error", "网络有点问题 :(", semantic, null).getBytes()));
                        } else {
                             addMessageToDB(new Message(AIUI, TEXT,
                                     fakeSemanticResult(0, "error", aiuiEvent.arg1 + " 错误", semantic, null).getBytes()));
                        }
                    }
                    break;
                    case AIUIConstant.EVENT_CMD_RETURN: {
                        processCmdReturnEvent(aiuiEvent);
                    }

                    case AIUIConstant.EVENT_CONNECTED_TO_SERVER: {
                        mUID.postValue(aiuiEvent.data.getString("uid"));
                    }
                    break;

                    case AIUIConstant.EVENT_VAD: {
                        mVADEvent.postValue(aiuiEvent);
                    }
                    break;
                }
            }
        };

        //监听设置变化
        mSettingsRepo.getSettings().observeForever(new Observer<Settings>() {
            @Override
            public void onChanged(@Nullable Settings settings) {
                mCurrentSettings = settings;
                //配置变化，重新创建AIUIAgent
                if(!compare(settings, mLastConfig) || mAgent == null) {
                    try {
                        mLastConfig.optJSONObject("login").put("appid", mCurrentSettings.appid);
                        mLastConfig.optJSONObject("login").put("key", mCurrentSettings.key);
                        mLastConfig.optJSONObject("global").put("scene", mCurrentSettings.scene);
                        mLastConfig.optJSONObject("vad").put(AIUIConstant.KEY_VAD_EOS, String.valueOf(mCurrentSettings.eos));
                        mLastConfig.optJSONObject("log").put("debug_log", mCurrentSettings.debugLog?"1":"0");
                        mLastConfig.optJSONObject("log").put("save_datalog", mCurrentSettings.saveDebugLog?"1":"0");
                        if(settings.wakeup){
                            mLastConfig.optJSONObject("speech").put("wakeup_mode", "ivw");
                            mLastConfig.optJSONObject("speech").put("interact_mode", "oneshot");

                            JSONObject ivw = new JSONObject();
                            ivw.put("res_path", "ivw/ivw.jet");
                            ivw.put("res_type", "assets");
                            mLastConfig.put("ivw", ivw);
                        } else {
                            mLastConfig.optJSONObject("speech").put("wakeup_mode", "off");
                            mLastConfig.optJSONObject("speech").put("interact_mode", "continuous");
                        }

                        if(mAgent != null) {
                            mAgent.destroy();
                        }
                        initAIUIAgent(mContext, mLastConfig);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }




    public LiveData<List<Message>> getInteractMessages(){
        return mInteractMsgFlowable;
    }

    //文本语义
    public void writeText(String message){
        if(mAppendVoiceMsg != null){
            //更新上一条未完成的语音消息内容
            updateMessage(mAppendVoiceMsg);
            mAppendVoiceMsg = null;
        }
        sendMessage(new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0,
                "data_type=text", message.getBytes()));
        addMessageToDB(new Message(USER, TEXT, message.getBytes()));
    }private void processResult(AIUIEvent event) {
        try {
            JSONObject bizParamJson = new JSONObject(event.info);
            JSONObject data = bizParamJson.getJSONArray("data").getJSONObject(0);
            JSONObject params = data.getJSONObject("params");
            JSONObject content = data.getJSONArray("content").getJSONObject(0);

            if (content.has("cnt_id")) {
                String cnt_id = content.getString("cnt_id");
                JSONObject cntJson = new JSONObject(new String(event.data.getByteArray(cnt_id), "utf-8"));

                String sub = params.optString("sub");
                if ("nlp".equals(sub)) {
                    // 解析得到语义结果
                    JSONObject semanticResult = cntJson.optJSONObject("intent");
                    if (semanticResult != null && semanticResult.length() != 0) {
                        addMessageToDB(new Message(AIUI, TEXT, semanticResult.toString().getBytes()));
                    }
                } else if("iat".equals(sub)) {
                    //根据听写结果更新当前语音消息的内容
                    updateVoiceMessageFromIAT(cntJson);
                }
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }
    //处理听写PGS的队列
    private String[] mIATPGSStack = new String[256];
    private List<String> mInterResultStack = new ArrayList<>();
    private void updateVoiceMessageFromIAT(JSONObject cntJson) throws JSONException {
        if(mAppendVoiceMsg == null) return;

        JSONObject text = cntJson.optJSONObject("text");
        // 解析拼接此次听写结果
        StringBuilder iatText = new StringBuilder();
        JSONArray words = text.optJSONArray("ws");
        boolean lastResult = text.optBoolean("ls");
        for(int index = 0; index < words.length(); index++) {
            JSONArray charWord = words.optJSONObject(index).optJSONArray("cw");
            for(int cIndex = 0; cIndex < charWord.length(); cIndex++) {
                iatText.append(charWord.optJSONObject(cIndex).opt("w"));
            }
        }


        String voiceIAT = "";
        String pgsMode = text.optString("pgs");
        //非PGS模式结果
        if(TextUtils.isEmpty(pgsMode)) {
            if(TextUtils.isEmpty(iatText)) return;

            //和上一次结果进行拼接
            if(!TextUtils.isEmpty(mAppendVoiceMsg.cacheContent)) {
                voiceIAT = mAppendVoiceMsg.cacheContent + "\n";
            }
            voiceIAT += iatText;
        } else {
            int serialNumber = text.optInt("sn");
            mIATPGSStack[serialNumber] = iatText.toString();
            //pgs结果两种模式rpl和apd模式（替换和追加模式）
            if("rpl".equals(pgsMode)) {
                //根据replace指定的range，清空stack中对应位置值
                JSONArray replaceRange = text.optJSONArray("rg");
                int start = replaceRange.getInt(0);
                int end = replaceRange.getInt(1);

                for(int index = start; index <= end; index++) {
                    mIATPGSStack[index] = null;
                }
            }

            StringBuilder PGSResult = new StringBuilder();
            //汇总stack经过操作后的剩余的有效结果信息
            for(int index = 0; index < mIATPGSStack.length; index++) {
                if(TextUtils.isEmpty(mIATPGSStack[index])) continue;

                if(!TextUtils.isEmpty(PGSResult.toString())) PGSResult.append("\n");
                PGSResult.append(mIATPGSStack[index]);
                //如果是最后一条听写结果，则清空stack便于下次使用
                if(lastResult) {
                    mIATPGSStack[index] = null;
                }
            }
            voiceIAT = join(mInterResultStack) + PGSResult.toString();

            if(lastResult) {
                mInterResultStack.add(PGSResult.toString());
            }
        }

        if (!TextUtils.isEmpty(voiceIAT)) {
            mAppendVoiceMsg.cacheContent = voiceIAT;
            updateMessage(mAppendVoiceMsg);
        }
    }

    public void startRecord() {
        startRecordAudio();
        beginAudio();
    }

    public void stopRecord() {
        stopRecordAudio();
        endAudio();
    }

    //模拟AIUI结果信息
    public Message fakeAIUIResult(int rc, String service, String answer,
                               Map<String, String> semantic, Map<String, String> mapData){

        Message msg = new Message(AIUI, TEXT,
                fakeSemanticResult(rc, service, answer, semantic, mapData).getBytes());
        addMessageToDB(msg);
        return msg;
    }

    //手动设置位置信息
    public void setLoc(double lon, double lat){
        try {
            JSONObject audioParams = new JSONObject();
            audioParams.put("msc.lng", String.valueOf(lon));
            audioParams.put("msc.lat", String.valueOf(lat));

            JSONObject params = new JSONObject();
            params.put("audioparams", audioParams);

            //完成设置后，在随后的每次会话都会携带该位置信息
            setParams(params.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void setPersParams(JSONObject persParams) {
        try {
            //参考文档动态实体生效使用一节
            JSONObject params = new JSONObject();
            JSONObject audioParams = new JSONObject();
            audioParams.put("pers_param", persParams.toString());
            params.put("audioparams", audioParams);

            setParams(params.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public LiveData<String> getUID() {
        return mUID;
    }

    public LiveData<AIUIEvent> getVADEvent() {
        return mVADEvent;
    }

    public LiveData<AIUIEvent> getStateEvent() {
        return mStateEvent;
    }

    public void updateMessage(final Message message){
        if(message == null) return;
        Completable
                .complete()
//                .delay(1, TimeUnit.SECONDS)
                .observeOn(Schedulers.io())
                .subscribe(new Action() {
                    @Override
                    public void run() throws Exception {
                        mMessageDao.updateMessage(message);
                    }
                });
    }

    private void initAIUIAgent(Context context, JSONObject config) {
        mAgent = AIUIAgent.createAgent(context, config.toString(), mAIUIListener);
        mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_START, 0, 0, null, null));

        if(mCurrentSettings.wakeup) {
            startRecordAudio();
        } else {
            stopRecordAudio();
        }
    }

    private void setParams(String params) {
        sendMessage(new AIUIMessage(AIUIConstant.CMD_SET_PARAMS, 0 , 0, params, null));
    }


    private void beginAudio() {
        mAudioStart = System.currentTimeMillis();
        if(mAppendVoiceMsg != null ){
            //更新上一条未完成的语音消息内容
            updateMessage(mAppendVoiceMsg);
            mAppendVoiceMsg = null;
            mInterResultStack.clear();
        }
        mAppendVoiceMsg = new Message(USER, Voice, new byte[]{});
        mAppendVoiceMsg.cacheContent = "";
        //语音消息msgData为录音时长
        mAppendVoiceMsg.msgData = ByteBuffer.allocate(4).putFloat(0).array();
        addMessageToDB(mAppendVoiceMsg);
    }

    private void endAudio() {
        if (mAppendVoiceMsg != null) {
            mAppendVoiceMsg.msgData = ByteBuffer.allocate(4).putFloat((System.currentTimeMillis() - mAudioStart)/1000.0f).array();
            updateMessage(mAppendVoiceMsg);
        }
    }

    private void stopRecordAudio() {
        sendMessage(new AIUIMessage(AIUIConstant.CMD_STOP_RECORD, 0, 0, "data_type=audio,sample_rate=16000", null));
    }

    private void startRecordAudio() {
        sendMessage(new AIUIMessage(AIUIConstant.CMD_START_RECORD, 0, 0, "data_type=audio,sample_rate=16000", null));
    }

    //解析AIUI语义结果信息
    private JSONObject getSemanticResult(AIUIEvent event){
        try {
            JSONObject bizParamJson = new JSONObject(event.info);
            JSONObject data = bizParamJson.getJSONArray("data").getJSONObject(0);
            JSONObject params = data.getJSONObject("params");
            JSONObject content = data.getJSONArray("content").getJSONObject(0);

            if (content.has("cnt_id")) {
                String cnt_id = content.getString("cnt_id");
                JSONObject cntJson = new JSONObject(new String(event.data.getByteArray(cnt_id), "utf-8"));

                String sub = params.optString("sub");
                if ("nlp".equals(sub)) {
                    // 解析得到语义结果
                   return cntJson.optJSONObject("intent");
                }
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }

        return null;
    }

    //解析AIUI听写结果信息
    private String getIATResult(AIUIEvent event){
        try {
        } catch (Throwable e) {
            e.printStackTrace();
        }

        return null;
    }


    private void sendMessage(AIUIMessage message){
        if(mAgent != null) {
            //确保AIUI处于唤醒状态
            if(mCurrentState != AIUIConstant.STATE_WORKING && !mCurrentSettings.wakeup){
                mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0, 0, "", null));
            }

            mAgent.sendMessage(message);
        }
    }

    private String fakeSemanticResult(int rc, String service, String answer,
                                      Map<String, String> semantic,
                                      Map<String, String> mapData){
        try {
            JSONObject data = new JSONObject();
            if(mapData != null){
                for(String key : mapData.keySet()){
                    data.put(key, mapData.get(key));
                }
            }

            JSONObject semanticData = new JSONObject();
            if(semantic != null){
                for(String key : semantic.keySet()){
                    semanticData.put(key, semantic.get(key));
                }
            }


            JSONObject answerData = new JSONObject();
            answerData.put("text", answer);


            JSONObject fakeResult = new JSONObject();
            fakeResult.put("rc", rc);
            fakeResult.put("answer", answerData);
            fakeResult.put("service", service);
            fakeResult.put("semantic", semanticData);
            fakeResult.put("data", data);

            return fakeResult.toString();
        } catch (JSONException e) {
            e.printStackTrace();
            return null;
        }

    }

    private void addMessageToDB(final Message msg){
        Completable
                .complete()
                .observeOn(Schedulers.io())
                .subscribe(new Action() {
            @Override
            public void run() throws Exception {
                mMessageDao.addMessage(msg);
            }
        });

    }



    private boolean compare(Settings settings, JSONObject config) {
        return settings.eos == Integer.valueOf(config.optJSONObject("vad").optString(AIUIConstant.KEY_VAD_EOS, "1000"))
                && settings.debugLog == ("1".equals(config.optJSONObject("log").optString("debug_log")))
                && settings.saveDebugLog == ("1".equals(config.optJSONObject("log").optString("save_datalog")))
                && settings.wakeup == ("ivw".equals(config.optJSONObject("speech").optString("wakeup_mode")))
                && settings.appid.equals(config.optJSONObject("login").optString("appid"))
                && settings.key.equals(config.optJSONObject("login").optString("key"))
                && settings.scene.equals(config.optJSONObject("global").optString("scene"));
    }


    //同步所见即可说
    public void syncSpeakableData(SpeakableSyncData data){
        try {
            JSONObject syncSpeakableJson = new JSONObject();

            //从所见即可说数据中根据key获取识别热词信息
            List<String> hotWords = new ArrayList<>();
            String[] dataItems = data.speakableData.split("\r?\n");
            for(String item : dataItems){
                JSONObject dataItem = new JSONObject(item);
                Iterator<String> hotKeysIterator;
                if(data.masterKey == null){
                    hotKeysIterator = dataItem.keys();
                }else{
                    List<String> hotKeys = new ArrayList<>();
                    hotKeys.add(data.masterKey);
                    hotKeys.add(data.subKeys);
                    hotKeysIterator = hotKeys.iterator();
                }

                while(hotKeysIterator.hasNext()){
                    String hotKey = hotKeysIterator.next();
                    hotWords.add(dataItem.getString(hotKey));
                }
            }

            // 识别用户数据
            JSONObject iatUserDataJson = new JSONObject();
            iatUserDataJson.put("recHotWords", TextUtils.join("|", hotWords));
            iatUserDataJson.put("sceneInfo", new JSONObject());
            syncSpeakableJson.put("iat_user_data", iatUserDataJson);

            // 语义理解用户数据
            JSONObject nlpUserDataJson = new JSONObject();

            JSONArray resArray = new JSONArray();
            JSONObject resDataItem = new JSONObject();
            resDataItem.put("res_name", data.resName);
            resDataItem.put("data", Base64.encodeToString(
                    data.speakableData.getBytes(),  Base64.NO_WRAP));
            resArray.put(resDataItem);

            nlpUserDataJson.put("res", resArray);
            nlpUserDataJson.put("skill_name", data.skillName);

            syncSpeakableJson.put("nlp_user_data", nlpUserDataJson);

            // 传入的数据一定要为utf-8编码
            byte[] syncData = syncSpeakableJson.toString().getBytes("utf-8");

            AIUIMessage syncAthenaMessage = new AIUIMessage(AIUIConstant.CMD_SYNC,
                    AIUIConstant.SYNC_DATA_SPEAKABLE, 0, "", syncData);

            sendMessage(syncAthenaMessage);
        } catch (Exception e) {
            e.printStackTrace();
            addMessageToDB(new Message(AIUI, TEXT,
                    String.format("同步所见即可说数据出错 %s", e.getMessage()).getBytes()));
        }
    }

    //同步动态实体
    public void syncDynamicEntity(DynamicEntityData data){
        try {
            // schema数据同步
            JSONObject syncSchemaJson = new JSONObject();
            JSONObject paramJson = new JSONObject();

            paramJson.put("id_name", data.idName);
            paramJson.put("id_value", data.idValue);
            paramJson.put("res_name", data.resName);

            syncSchemaJson.put("param", paramJson);
            syncSchemaJson.put("data", Base64.encodeToString(
                    data.syncData.getBytes(), Base64.DEFAULT | Base64.NO_WRAP));

            // 传入的数据一定要为utf-8编码
            byte[] syncData = syncSchemaJson.toString().getBytes("utf-8");

            AIUIMessage syncAthenaMessage = new AIUIMessage(AIUIConstant.CMD_SYNC,
                    AIUIConstant.SYNC_DATA_SCHEMA, 0, "", syncData);
            sendMessage(syncAthenaMessage);
        } catch (Exception e) {
            e.printStackTrace();
            addMessageToDB(new Message(AIUI, TEXT,
                    String.format("上传动态实体数据出错 %s", e.getMessage()).getBytes()));
        }
    }

    //查询动态实体打包状态
    public void queryDynamicSyncStatus(String sid){
        JSONObject paramsJson = new JSONObject();
        try {
            paramsJson.put("sid", sid);
            AIUIMessage querySyncMsg = new AIUIMessage(AIUIConstant.CMD_QUERY_SYNC_STATUS,
                    AIUIConstant.SYNC_DATA_SCHEMA, 0,
                    paramsJson.toString(), null);
            sendMessage(querySyncMsg);

        } catch (JSONException e) {
            e.printStackTrace();
            addMessageToDB(new Message(AIUI, TEXT,
                    String.format("查询动态实体数据同步状态出错 %s", e.getMessage()).getBytes()));
        }
    }

    private void processCmdReturnEvent(AIUIEvent event){
        int cmdType = event.arg1;
        switch (cmdType){
            case AIUIConstant.CMD_SYNC:{
                int syncType = event.data.getInt("sync_dtype");
                int resultCode = event.arg2;

                if (AIUIConstant.SYNC_DATA_SCHEMA == syncType) {
                    //动态实体上传结果
                    String sid = event.data.getString("sid");
                    Map<String, String> dynamicRet = new HashMap<>();
                    dynamicRet.put("sid", sid);
                    dynamicRet.put("ret", String.valueOf(resultCode));
                    addMessageToDB(new Message(AIUI, TEXT,
                            fakeSemanticResult(0, Constant.DYNAMIC,
                                    String.format("上传动态实体数据%s",
                                            resultCode==0? "成功":"失败"),
                                    null, dynamicRet).getBytes()));

                } else if (AIUIConstant.SYNC_DATA_SPEAKABLE == syncType) {
                    //所见即可说上传结果
                    addMessageToDB(new Message(AIUI, TEXT,
                            fakeSemanticResult(0, Constant.SPEAKABLE,
                                    String.format("可见即可说数据同步 %s", resultCode==0? "成功":"失败"),
                                    null, null).getBytes()));
                }
            }
            break;

            case AIUIConstant.CMD_QUERY_SYNC_STATUS:{
                int syncType = event.data.getInt("sync_dtype");

                if (AIUIConstant.SYNC_DATA_QUERY == syncType) {
                    //动态实体打包状态查询结果
                    String result = event.data.getString("result");
                    int resultCode = event.arg2;

                    Map<String, String> mapData = new HashMap<>();
                    mapData.put("ret", String.valueOf(resultCode));
                    mapData.put("result", result);
                    addMessageToDB(new Message(AIUI, TEXT,
                            fakeSemanticResult(0, Constant.DYNAMIC_QUERY,
                                    String.format("动态实体数据状态查询结果 %s", resultCode==0? "成功":"失败"),
                                    null, null).getBytes()));
                }
            }

        }
    }

    private String join(List<String> data) {
        StringBuilder builder = new StringBuilder();
        for(int index=0; index < data.size(); index++) {
            builder.append(data.get(index));
            builder.append("\n");
        }

        return builder.toString();
    }
}

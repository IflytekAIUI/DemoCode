package com.iflytek.aiuidemosample.repository;

import android.arch.persistence.room.Room;
import android.content.Context;
import android.text.TextUtils;
import android.util.Base64;

import com.iflytek.aiui.AIUIAgent;
import com.iflytek.aiui.AIUIConstant;
import com.iflytek.aiui.AIUIEvent;
import com.iflytek.aiui.AIUIListener;
import com.iflytek.aiui.AIUIMessage;
import com.iflytek.aiuidemosample.common.Constant;
import com.iflytek.aiuidemosample.db.MessageDB;
import com.iflytek.aiuidemosample.db.MessageDao;
import com.iflytek.aiuidemosample.model.data.DynamicEntityData;
import com.iflytek.aiuidemosample.model.Message;
import com.iflytek.aiuidemosample.model.data.SpeakableSyncData;
import com.iflytek.cloud.SpeechUtility;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import io.reactivex.Completable;
import io.reactivex.Flowable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.functions.Action;
import io.reactivex.schedulers.Schedulers;

import static com.iflytek.aiuidemosample.model.Message.FromType.AIUI;
import static com.iflytek.aiuidemosample.model.Message.FromType.USER;
import static com.iflytek.aiuidemosample.model.Message.MsgType.TEXT;
import static com.iflytek.aiuidemosample.model.Message.MsgType.Voice;

/**
 * Created by PR on 2017/7/26.
 */

public class MessagesCenterRepository {
    private AIUIAgent mAgent;

    private Flowable<List<Message>> mInteractMsgFlowable;
    private Message mAppendVoiceMsg = null;

    private MessageDB mDB;
    private MessageDao mMessageDao;

    private int mCurrentState = AIUIConstant.STATE_IDLE;

    private String mAppid;
    private String mKey;

    public MessagesCenterRepository(Context context, JSONObject config){

        mAppid = config.optJSONObject("login").optString("appid");
        mKey = config.optJSONObject("login").optString("key");

        mDB = Room.databaseBuilder(context, MessageDB.class, "aiui_message").build();
        mMessageDao = mDB.messageDao();

        mInteractMsgFlowable = mMessageDao.getAllMessage().observeOn(AndroidSchedulers.mainThread());

        SpeechUtility.createUtility(context,
                String.format("engine_mode=msc,delay_init=0,appid=%s,key=%s",
                        mAppid, mKey));

        mAgent = AIUIAgent.createAgent(context, config.toString(), new AIUIListener() {
            @Override
            public void onEvent(AIUIEvent aiuiEvent) {
                switch (aiuiEvent.eventType) {
                    case AIUIConstant.EVENT_STATE: {
                        mCurrentState = aiuiEvent.arg1;
                    }
                    break;
                    case AIUIConstant.EVENT_RESULT: {
                        JSONObject semanticResult = getSemanticResult(aiuiEvent);
                        if (semanticResult != null && semanticResult.length() != 0) {
                            addMessageToDB(new Message(AIUI, TEXT, semanticResult.toString().getBytes()));
                            if (mAppendVoiceMsg != null) {
                                mAppendVoiceMsg.cacheContent += semanticResult.optString("text");
                                updateMessage(mAppendVoiceMsg);
                            }
                        }
                    }
                    break;
                    case AIUIConstant.EVENT_SLEEP: {
                        addMessageToDB(new Message(AIUI, TEXT,
                                fakeSemanticResult(0, "sleep", "都不理我，我去休息了", null, null).getBytes()));
                    }
                    break;

                    case AIUIConstant.EVENT_ERROR: {
                        if (aiuiEvent.arg1 == 10120) {
                            addMessageToDB(new Message(AIUI, TEXT,
                                    fakeSemanticResult(0, "error", "网络有点问题 :(", null, null).getBytes()));
                        }
                    }
                    break;
                    case AIUIConstant.EVENT_CMD_RETURN: {
                        processCmdReturnEvent(aiuiEvent);
                    }
                    break;
                }
            }
        });
        mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_START, 0, 0, null, null));
        mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0, 0, "", null));
    }

    public Flowable<List<Message>> getInteractMessages(){
        return mInteractMsgFlowable;
    }

    public void writeText(String message){
        sendMessage(new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0,
                "data_type=text", message.getBytes()));
        addMessageToDB(new Message(USER, TEXT, message.getBytes()));
    }

    public void startWriteAudio(){
        if(mAppendVoiceMsg != null){
            updateMessage(mAppendVoiceMsg);
            mAppendVoiceMsg = null;
        }
    }

    public void writeAudio(byte[] audio, String audioParams) {
        sendMessage(new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0,
                "data_type=audio,sample_rate=16000," + audioParams, audio));
        if(mAppendVoiceMsg == null){
            mAppendVoiceMsg = new Message(USER, Voice, audio);
            addMessageToDB(mAppendVoiceMsg);
        }else {
            byte[] oldBuf = mAppendVoiceMsg.msgData;
            byte[] newBuf = new byte[audio.length + oldBuf.length];
            System.arraycopy(oldBuf, 0, newBuf, 0 , oldBuf.length);
            System.arraycopy(audio, 0, newBuf, oldBuf.length, audio.length);
            mAppendVoiceMsg.msgData = newBuf;
        }
    }

    public void stopWriteAudio(){
        sendMessage(new AIUIMessage(AIUIConstant.CMD_STOP_WRITE, 0, 0,
                "data_type=audio,sample_rate=16000", null));
        if(mAppendVoiceMsg != null){
            updateMessage(mAppendVoiceMsg);
        }
    }

    public void fakeAIUIResult(int rc, String service, String answer,
                               Map<String, String> semantic, Map<String, String> mapData){

        addMessageToDB(new Message(AIUI, TEXT,
                fakeSemanticResult(rc, service, answer, semantic, mapData).getBytes()));
    }

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

            nlpUserDataJson.put("appid", mAppid);
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


    public void syncDynamicEntity(DynamicEntityData data){
        try {
            // schema数据同步
            JSONObject syncSchemaJson = new JSONObject();
            JSONObject paramJson = new JSONObject();

            paramJson.put("appid", mAppid);
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

    private void processCmdReturnEvent(AIUIEvent event){
        int cmdType = event.arg1;
        switch (cmdType){
            case AIUIConstant.CMD_SYNC:{
                int syncType = event.data.getInt("sync_dtype");
                int resultCode = event.arg2;

                if (AIUIConstant.SYNC_DATA_SCHEMA == syncType) {
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

    private void sendMessage(AIUIMessage message){
        if(mCurrentState != AIUIConstant.STATE_WORKING){
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0, 0, "", null));
        }

        mAgent.sendMessage(message);
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

    private void updateMessage(final Message message){
        Completable
                .complete()
                .observeOn(Schedulers.io())
                .subscribe(new Action() {
                    @Override
                    public void run() throws Exception {
                        mMessageDao.updateMessage(message);
                    }
                });
    }


}

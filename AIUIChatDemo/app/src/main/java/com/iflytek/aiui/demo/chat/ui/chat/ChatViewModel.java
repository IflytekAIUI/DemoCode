package com.iflytek.aiui.demo.chat.ui.chat;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.Observer;
import android.arch.lifecycle.ViewModel;
import android.content.Context;
import android.support.annotation.Nullable;

import com.iflytek.aiui.AIUIEvent;
import com.iflytek.aiui.demo.chat.model.Message;
import com.iflytek.aiui.demo.chat.model.Settings;
import com.iflytek.aiui.demo.chat.model.data.DynamicEntityData;
import com.iflytek.aiui.demo.chat.model.data.SpeakableSyncData;
import com.iflytek.aiui.demo.chat.repository.AIUIRepository;
import com.iflytek.aiui.demo.chat.repository.ContactRepository;
import com.iflytek.aiui.demo.chat.repository.LocationRepo;
import com.iflytek.aiui.demo.chat.repository.SettingsRepo;
import com.iflytek.aiui.demo.chat.repository.Storage;
import com.iflytek.location.result.GPSLocResult;
import com.iflytek.location.result.NetLocResult;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.inject.Inject;

/**
 * Created by PR on 2017/7/26.
 */

public class ChatViewModel extends ViewModel {
    private ContactRepository mContactRepository;
    private AIUIRepository mRepository;
    private Storage mStorage;
    private Context mContext;
    private SettingsRepo mSettingsRepo;
    private LocationRepo mLocRepo;

    private JSONObject mPersParams;

    @Inject
    public ChatViewModel(Context context, SettingsRepo settingsRepo,
                         Storage storage,
                         AIUIRepository messagesRepository,
                         ContactRepository contactRepository,
                         LocationRepo locationRepo) {
        mContext = context;
        mStorage = storage;
        mRepository = messagesRepository;
        mContactRepository = contactRepository;
        mSettingsRepo = settingsRepo;
        mLocRepo = locationRepo;
        mPersParams = new JSONObject();
    }

    public LiveData<List<Message>> getInteractMessages() {
        return mRepository.getInteractMessages();
    }

    //发送文本交互消息
    public void sendText(String msg) {
        mRepository.writeText(msg);
    }

    //AIUI开始录音
    public void startRecord() {
        mRepository.startRecord();
    }

    //AIUI停止录音
    public void stopRecord() {
        mRepository.stopRecord();
    }

    //生效动态实体
    public void putPersParam(String key, String value) {
        try {
            mPersParams.put(key, value);
            mRepository.setPersParams(mPersParams);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    //模拟AIUI结果信息，用于展示如欢迎语或者操作结果信息
    public Message fakeAIUIResult(int rc, String service, String answer) {
        return mRepository.fakeAIUIResult(rc, service, answer, null, null);
    }

    //更新消息列表中特定的消息内容
    public void updateMessage(Message msg) {
        mRepository.updateMessage(msg);
    }

    //上传动态实体数据
    public void syncDynamicData(DynamicEntityData data) {
        mRepository.syncDynamicEntity(data);
    }

    //查询动态实体上传状态
    public void queryDynamicStatus(String sid) {
        mRepository.queryDynamicSyncStatus(sid);
    }

    //上传所见即可说数据
    public void syncSpeakableData(SpeakableSyncData data) {
        mRepository.syncSpeakableData(data);
    }

    public List<String> getContacts() {
        return mContactRepository.getContacts();
    }

    public LiveData<Settings> getSettings() {
        return mSettingsRepo.getSettings();
    }

    public LiveData<AIUIEvent> getVADEvent() {
        return mRepository.getVADEvent();
    }

    public LiveData<AIUIEvent> getStateEvent() {
        return mRepository.getStateEvent();
    }

    public Context getContext(){
        return mContext;
    }

    public String readAssetFile(String filename){
        return mStorage.readAssetFile(filename);
    }

    public void useLocationData() {
        mLocRepo.getNetLoc().observeForever(new Observer<NetLocResult>() {
            @Override
            public void onChanged(@Nullable NetLocResult netLoc) {
                mRepository.setLoc(netLoc.getLon(), netLoc.getLat());

                String location = String.format("net location city %s, lon %f lat %f", netLoc.getCity(), netLoc.getLon(), netLoc.getLat());
                Map<String, String> data = new HashMap<>();
                data.put("netLoc", location);
                mRepository.fakeAIUIResult(0, "fake.Loc", "已获取使用最新的网络位置信息", null, data);
            }
        });

        mLocRepo.getGPSLoc().observeForever(new Observer<GPSLocResult>() {
            @Override
            public void onChanged(@Nullable GPSLocResult gpsLoc) {
                mRepository.setLoc(gpsLoc.getLon(), gpsLoc.getLat());

                String location = String.format("GPS location lon %f lat %f", gpsLoc.getLon(), gpsLoc.getLat());
                Map<String, String> data = new HashMap<>();
                data.put("gpsLoc", location);
                mRepository.fakeAIUIResult(0, "fake.Loc", "已获取使用最新的GPS位置", null, data);
            }
        });
    }

    public void useNewAppID(String appid, String key, String scene) {
        mSettingsRepo.config(appid, key, scene);
    }


}

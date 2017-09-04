package com.iflytek.aiuidemosample.viewmodel;

import android.Manifest;
import android.app.Activity;
import android.arch.lifecycle.ViewModel;
import android.content.Context;

import com.iflytek.aiuidemosample.model.data.DynamicEntityData;
import com.iflytek.aiuidemosample.model.Message;
import com.iflytek.aiuidemosample.model.data.SpeakableSyncData;
import com.iflytek.aiuidemosample.model.handler.Handler;
import com.iflytek.aiuidemosample.repository.ContactRepository;
import com.iflytek.aiuidemosample.repository.MessagesCenterRepository;
import com.karumi.dexter.Dexter;
import com.karumi.dexter.PermissionToken;
import com.karumi.dexter.listener.PermissionGrantedResponse;
import com.karumi.dexter.listener.PermissionRequest;
import com.karumi.dexter.listener.single.BasePermissionListener;
import com.karumi.dexter.listener.single.CompositePermissionListener;
import com.karumi.dexter.listener.single.DialogOnDeniedPermissionListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import io.reactivex.Flowable;

/**
 * Created by PR on 2017/7/26.
 */

public class MessagesViewModel extends ViewModel {
    private ContactRepository mContactRepository;
    private MessagesCenterRepository mRepository;
    private Activity activity;
    private Map<String, String> persParamsMap = new HashMap<>();

    private static MessagesViewModel sInstance;

    public static void init(Activity activity, MessagesCenterRepository repository) {
        sInstance = new MessagesViewModel(activity, repository);
    }

    public static MessagesViewModel getInstance() {
        return sInstance;
    }

    public Context getContext() {
        return activity;
    }

    private MessagesViewModel(Activity activity, MessagesCenterRepository repository) {
        this.activity = activity;
        mRepository = repository;
        mContactRepository = new ContactRepository(activity);
    }

    public Flowable<List<Message>> getInteractMessages() {
        return mRepository.getInteractMessages();
    }

    //发送文本交互消息
    public void sendText(String msg) {
        mRepository.writeText(msg);
    }

    public void startWriteAudio() {
        mRepository.startWriteAudio();
    }

    public void writeAudio(byte[] audio) {
        //组建pers_param参数
        JSONObject params = new JSONObject();
        try {
            for (Map.Entry<String, String> entry : persParamsMap.entrySet()) {
                params.put(entry.getKey(), entry.getValue());
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        mRepository.writeAudio(audio, String.format("pers_param=%s", params.toString()));
    }

    public void stopWriteAudio() {
        mRepository.stopWriteAudio();
    }

    public void putPersParam(String key, String value) {
        persParamsMap.put(key, value);
    }

    public void fakeAIUIResult(int rc, String service, String answer) {
        mRepository.fakeAIUIResult(rc, service, answer, null, null);
    }

    public void syncDynamicData(DynamicEntityData data) {
        mRepository.syncDynamicEntity(data);
    }

    public void queryDynamicStatus(String sid) {
        mRepository.queryDynamicSyncStatus(sid);
    }

    public void syncSpeakableData(SpeakableSyncData data) {
        mRepository.syncSpeakableData(data);
    }

    public void checkContactUpdate() {
        Dexter.withActivity(activity)
                .withPermission(Manifest.permission.READ_CONTACTS)
                .withListener(new CompositePermissionListener(new BasePermissionListener() {
                    @Override
                    public void onPermissionGranted(PermissionGrantedResponse response) {
                        StringBuilder contactJson = new StringBuilder();
                        if (mContactRepository.hasChanged()) {
                            fakeAIUIResult(0, "fake",
                                    "检测到通讯录变化，上传通讯录联系人"
                                            + Handler.NEWLINE
                                            + Handler.NEWLINE
                                            + "上传成功后，即可通过 打电话给XXX 进行交互"
                            );

                            List<String> contacts = mContactRepository.getContacts();
                            for (String contact : contacts) {
                                String[] nameNumber = contact.split("\\$\\$");
                                contactJson.append(String.format("{\"name\": \"%s\", \"phoneNumber\": \"%s\" }\n",
                                        nameNumber[0], nameNumber[1]));
                            }

                            syncDynamicData(new DynamicEntityData(
                                    "IFLYTEK.telephone_contact", "uid", "", contactJson.toString()));
                            putPersParam("uid", "");
                        }
                    }

                    @Override
                    public void onPermissionRationaleShouldBeShown(PermissionRequest permission, PermissionToken token) {
                        token.continuePermissionRequest();
                        super.onPermissionRationaleShouldBeShown(permission, token);
                    }


                }, DialogOnDeniedPermissionListener.Builder
                        .withContext(activity)
                        .withTitle("读取通讯录")
                        .withMessage("该Demo中包含电话技能演示，需要该授予该权限")
                        .withButtonText(android.R.string.ok)
                        .build())).check();


    }

}

package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.Message;
import com.iflytek.aiui.demo.chat.model.data.DynamicEntityData;
import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import java.util.List;

import io.reactivex.Flowable;
import io.reactivex.functions.Consumer;
import io.reactivex.functions.Function;
import io.reactivex.functions.Predicate;
import io.reactivex.schedulers.Schedulers;

/**
 * Created by PR on 2017/12/18.
 */

public class TelephoneHandler extends Handler {
    public TelephoneHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        if(result.answer.contains("没有为您找到")) {
            StringBuilder builder = new StringBuilder();
            builder.append(result.answer);
            builder.append(NEWLINE);
            builder.append(NEWLINE);
            builder.append("<a href=\"upload_contact\">上传本地联系人数据</a>");
            return builder.toString();
        } else {
            return result.answer;
        }
    }

    @Override
    public boolean urlClicked(String url) {
        if ("upload_contact".equals(url)) {
            mPermissionChecker.checkPermission(android.Manifest.permission.READ_CONTACTS, new Runnable() {
                @Override
                public void run() {
                    uploadContacts();
                }
            }, null);
        }
        return super.urlClicked(url);
    }

    private void uploadContacts() {
        final Message progressMsg = mMessageViewModel.fakeAIUIResult(0, "contacts_upload", "上传进度10%");
        Flowable.fromArray(1)
                .observeOn(Schedulers.io())
                .map(new Function<Integer, List<String>>() {
                    @Override
                    public List<String> apply(Integer integer) throws Exception {
                        List<String> contacts = mMessageViewModel.getContacts();
                        progressMsg.msgData = new String(progressMsg.msgData).replace("10", "40").getBytes();
                        mMessageViewModel.updateMessage(progressMsg);

                        return contacts;
                    }
                })
                .filter(new Predicate<List<String>>() {
                    @Override
                    public boolean test(List<String> strings) throws Exception {
                        if(strings == null || strings.size() == 0){
                             mMessageViewModel.fakeAIUIResult(0, "contacts_upload", "请允许应用请求的联系人读取权限");
                            return false;
                        } else {
                            return true;
                        }
                    }
                })
                .map(new Function<List<String>, String>() {
                    @Override
                    public String apply(List<String> contacts) throws Exception {
                        StringBuilder contactJson = new StringBuilder();
                        for (String contact : contacts) {
                            String[] nameNumber = contact.split("\\$\\$");
                            contactJson.append(String.format("{\"name\": \"%s\", \"phoneNumber\": \"%s\" }\n",
                                    nameNumber[0], nameNumber[1]));
                        }
                        progressMsg.msgData = new String(progressMsg.msgData).replace("40", "70").getBytes();
                        mMessageViewModel.updateMessage(progressMsg);

                        return contactJson.toString();
                    }
                })
                .subscribe(new Consumer<String>() {
                    @Override
                    public void accept(String contacts) throws Exception {
                        mMessageViewModel.syncDynamicData(new DynamicEntityData(
                                "IFLYTEK.telephone_contact", "uid", "", contacts));
                        mMessageViewModel.putPersParam("uid", "");
                        progressMsg.msgData = new String(progressMsg.msgData).replace("70", "100").getBytes();
                        mMessageViewModel.updateMessage(progressMsg);
                    }
                });
    }
}

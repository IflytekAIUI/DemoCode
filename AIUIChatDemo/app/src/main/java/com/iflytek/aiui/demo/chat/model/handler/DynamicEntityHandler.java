package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import org.json.JSONException;

import java.util.concurrent.TimeUnit;

import io.reactivex.Completable;
import io.reactivex.functions.Action;
import io.reactivex.schedulers.Schedulers;

/**
 * Created by PR on 2017/8/7.
 */

public class DynamicEntityHandler extends Handler {


    public DynamicEntityHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        try {
            int ret = Integer.valueOf(result.data.getString("ret"));
            if(ret != 0){
                return "动态实体数据上传失败";
            }else {
                Completable
                        .complete()
                        .observeOn(Schedulers.io())
                        .delay(1000, TimeUnit.MILLISECONDS)
                        .subscribe(new Action() {
                            @Override
                            public void run() throws Exception {
                                mMessageViewModel.queryDynamicStatus(result.data.getString("sid"));
                            }
                        });

                return result.answer;
            }
        } catch (JSONException e) {
            e.printStackTrace();
            return "错误 " + e.getMessage();
        }
    }
}

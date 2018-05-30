package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;


/**
 * Created by PR on 2018/3/22.
 */

public class NotificationHandler extends Handler {
    public NotificationHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        StringBuilder answer = new StringBuilder(result.answer);

        if(result.data.has("appid")) {
            answer.append(NEWLINE);
            answer.append(NEWLINE);
            answer.append("<a href=\"use\">马上体验一下</a>");

            return answer.toString().replaceAll("\n", NEWLINE);
        }

        return answer.toString();
    }

    @Override
    public boolean urlClicked(String url) {
        if("use".equals(url)) {
            //每次获取，getFormatContent只会调用一次
            String appid = result.data.optString("appid", "");
            String key = result.data.optString("key", "");
            String scene = result.data.optString("scene", "main");

            mMessageViewModel.useNewAppID(appid, key, scene);
            mMessageViewModel.fakeAIUIResult(0, "notification",
                    "已切换使用新的AppID，赶快开始体验吧\n\n若需要恢复，可在设置中重新配置");
            return true;
        } else {
            return super.urlClicked(url);
        }
    }
}

package com.iflytek.aiui.demo.chat.model.handler;

import android.text.TextUtils;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

/**
 * Created by PR on 2017/12/19.
 */

public class HintHandler extends Handler {

    private final StringBuilder defaultAnswer;

    public HintHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
        defaultAnswer = new StringBuilder();
        defaultAnswer.append("你好，我不懂你的意思");
        defaultAnswer.append(Handler.NEWLINE_NO_HTML);
        defaultAnswer.append(Handler.NEWLINE_NO_HTML);
        defaultAnswer.append("在后台添加更多技能让我变得更强大吧 :D");
    }


//        defaultAnswer.append(Handler.NEWLINE);
//        defaultAnswer.append(Handler.NEWLINE);
//        defaultAnswer.append("<a href=\"more\">你也可以了解我的更多特性</a>");

    @Override
    public String getFormatContent() {
        if(TextUtils.isEmpty(result.answer)) {
            return defaultAnswer.toString();
        } else {
            return result.answer;
        }
    }

    @Override
    public boolean urlClicked(String url) {
        if ("more".equals(url)) {
            StringBuilder more = new StringBuilder();
            more.append("1. <a href=\"individual\">用户个性化</a>");
            mMessageViewModel.fakeAIUIResult(0, "unknown", more.toString());
        } else if ("individual".equals(url)) {
            StringBuilder more = new StringBuilder();
            more.append("1. <a href=\"telephone\">电话</a>");
            more.append(NEWLINE);
            more.append("通过上传本地的联系人信息，可以让电话技能更准确地理解您的意思");
            more.append(NEWLINE);
            more.append(NEWLINE);
            more.append("2. <a href=\"menu\">点菜</a>");
            more.append(NEWLINE);
            more.append("通过点菜的技能，根据用户选择的不同分店的菜单，提供精准的识别和语义");
            more.append(NEWLINE);
            more.append(NEWLINE);
            more.append("3. <a href=\"dish\">菜谱</a>");
            more.append(NEWLINE);
            more.append("通过菜谱技能，根据信源结果预测下次用户交互的热词，提高识别和语义的精准度");
            mMessageViewModel.fakeAIUIResult(0, "unknown", more.toString());
        } else if ("telephone".equals(url)) {
            mMessageViewModel.sendText("打电话给妈妈");
        } else if ("menu".equals(url)) {
            mMessageViewModel.sendText("我要点菜");
        } else if ("dish".equals(url)) {
            mMessageViewModel.sendText("有哪些川菜");
        }
        return super.urlClicked(url);
    }
}

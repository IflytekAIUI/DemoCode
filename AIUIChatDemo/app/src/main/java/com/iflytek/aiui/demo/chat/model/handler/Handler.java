package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;
import com.zzhoujay.richtext.callback.OnUrlClickListener;
import com.zzhoujay.richtext.callback.OnUrlLongClickListener;

/**
 * Created by PR on 2017/8/4.
 */

public abstract class Handler implements OnUrlClickListener, OnUrlLongClickListener {
    public static final String NEWLINE = "<br/>";
    public static final String NEWLINE_NO_HTML = "\n";
    public static final String CONTROL_TIP = "你可以通过语音控制暂停，播放，上一首，下一首哦";
    public static int controlTipReqCount = 0;

    protected SemanticResult result;
    protected ChatViewModel mMessageViewModel;
    protected PlayerViewModel mPlayer;
    protected PermissionChecker mPermissionChecker;
    public Handler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result){
        this.mPlayer = player;
        this.mMessageViewModel = model;
        this.mPermissionChecker = checker;
        this.result = result;
    }

    public abstract String getFormatContent();

    @Override
    public boolean urlClicked(String url) {
        return true;
    }

    @Override
    public boolean urlLongClick(String url) {
        return true;
    }

    public static boolean isNeedShowContrlTip() {
        return controlTipReqCount++ % 5 == 0;
    }

}

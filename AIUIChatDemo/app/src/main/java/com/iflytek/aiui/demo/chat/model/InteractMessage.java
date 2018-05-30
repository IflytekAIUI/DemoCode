package com.iflytek.aiui.demo.chat.model;

import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

/**
 * Created by PR on 2017/11/7.
 */

public class InteractMessage {
    private Message mMsgImpl;
    private MessageHandler mHandler;
    private ChatViewModel mModel;

    public InteractMessage(Message message, PermissionChecker checker, ChatViewModel viewModel, PlayerViewModel player) {
        mModel = viewModel;
        mMsgImpl = message;
        mHandler = new MessageHandler(viewModel, player, checker, message);
    }

    public Message getMessage() {
        return mMsgImpl;
    }

    public MessageHandler getHandler() {
        return mHandler;
    }

    public String getDisplayText() {
        if(mMsgImpl.cacheContent== null) {
            mMsgImpl.cacheContent = mHandler.getFormatMessage();
            mModel.updateMessage(mMsgImpl);
        }

        return mMsgImpl.cacheContent;
    }
}

package com.iflytek.aiui.demo.chat.ui.chat;

import com.iflytek.aiui.demo.chat.databinding.ChatItemBinding;
import com.iflytek.aiui.demo.chat.model.InteractMessage;

/**
 * Created by PR on 2017/8/8.
 */

public interface ItemListener {
    public void onMessageClick(InteractMessage msg, ChatItemBinding binding);
}

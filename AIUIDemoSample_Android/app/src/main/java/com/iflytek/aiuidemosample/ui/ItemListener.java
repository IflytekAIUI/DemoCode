package com.iflytek.aiuidemosample.ui;

import com.iflytek.aiuidemosample.databinding.ChatItemBinding;
import com.iflytek.aiuidemosample.model.Message;

/**
 * Created by PR on 2017/8/8.
 */

public interface ItemListener {
    public void onMessageClick(Message msg, ChatItemBinding binding);
}

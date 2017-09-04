package com.iflytek.aiuidemosample.ui;

import android.databinding.DataBindingUtil;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.iflytek.aiuidemosample.R;
import com.iflytek.aiuidemosample.databinding.ChatItemBinding;
import com.iflytek.aiuidemosample.model.Message;
import com.iflytek.aiuidemosample.ui.common.DataBoundListAdapter;
import com.zzhoujay.richtext.RichText;

/**
 * Created by PR on 2017/7/31.
 */

public class MessageListAdapter extends DataBoundListAdapter<Message, ChatItemBinding>
        implements ItemListener{
    @Override
    protected ChatItemBinding createBinding(ViewGroup parent, int viewType) {
        return DataBindingUtil.inflate(LayoutInflater.from(parent.getContext()),
                R.layout.chat_item, parent, false);

    }

    @Override
    protected void bind(ChatItemBinding binding, Message item) {
            binding.setMsg(item);
            binding.setBinding(binding);
            binding.setHandler(this);
            RichText
                    .fromHtml(item.toTextContent())
                    .urlClick(item.handler)
                    .into(binding.chatItemContentText);
    }

    @Override
    protected boolean areItemsTheSame(Message oldItem, Message newItem) {
        return oldItem.timestamp == newItem.timestamp;
    }

    @Override
    protected boolean areContentsTheSame(Message oldItem, Message newItem) {
        return oldItem.msgData == newItem.msgData;
    }

    @Override
    public void onMessageClick(Message msg, ChatItemBinding binding) {
        if(msg.msgType == Message.MsgType.Voice) {
            if(binding.chatItemVoice.getVisibility() == View.VISIBLE){
                AudioHandler.getsInstance().playAudioMessage(msg);
                binding.chatItemVoice.setVisibility(View.GONE);
                binding.chatItemContentText.setVisibility(View.VISIBLE);
            }else{
                binding.chatItemVoice.setVisibility(View.VISIBLE);
                binding.chatItemContentText.setVisibility(View.GONE);
            }

        }
    }
}

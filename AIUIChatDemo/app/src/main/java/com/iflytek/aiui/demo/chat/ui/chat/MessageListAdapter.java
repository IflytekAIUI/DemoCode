package com.iflytek.aiui.demo.chat.ui.chat;

import android.databinding.DataBindingUtil;
import android.text.Spannable;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import com.iflytek.aiui.demo.chat.R;
import com.iflytek.aiui.demo.chat.databinding.ChatItemBinding;
import com.iflytek.aiui.demo.chat.model.InteractMessage;
import com.zzhoujay.richtext.RichText;
import com.zzhoujay.richtext.ext.LongClickableLinkMovementMethod;

import java.util.Arrays;

/**
 * Created by PR on 2017/7/31.
 */

public class MessageListAdapter extends DataBoundListAdapter<InteractMessage, ChatItemBinding>
        implements ItemListener {
    private ChatFragment mFragment;
    public MessageListAdapter(ChatFragment fragment){
        mFragment = fragment;
    }
    @Override
    protected ChatItemBinding createBinding(ViewGroup parent, int viewType) {
        return DataBindingUtil.inflate(LayoutInflater.from(parent.getContext()),
                R.layout.chat_item, parent, false);

    }

    @Override
    protected void bind(final ChatItemBinding binding, InteractMessage item) {
            binding.setMsg(item);
            binding.setBinding(binding);
            binding.setHandler(this);
            String content = item.getDisplayText();
            if(content.contains("</a>")){
                RichText.from(content)
                        .urlClick(item.getHandler())
                        .into(binding.chatItemContentText);
                binding.chatItemContentText.setOnTouchListener(new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View view, MotionEvent motionEvent) {
                        return new LongClickableLinkMovementMethod()
                                .onTouchEvent(binding.chatItemContentText, (Spannable) binding.chatItemContentText.getText(), motionEvent);
                    }
                });
            } else {
                binding.chatItemContentText.setText(item.getDisplayText());
            }

    }

    @Override
    protected boolean areItemsTheSame(InteractMessage oldItem, InteractMessage newItem) {
        return oldItem.getMessage().timestamp == newItem.getMessage().timestamp;
    }

    @Override
    protected boolean areContentsTheSame(InteractMessage oldItem, InteractMessage newItem) {
        if(Arrays.equals(oldItem.getMessage().msgData, newItem.getMessage().msgData)) {
            String oldContent = oldItem.getMessage().cacheContent;
            String newContent = newItem.getMessage().cacheContent;
            return oldContent == null? newContent == null : oldContent.equals(newContent);
        } else {
            return false;
        }

    }

    @Override
    public void onMessageClick(InteractMessage msg, ChatItemBinding binding) {
//        if(msg.getMessage().msgType == Message.MsgType.Voice) {
//            AudioHandler.getsInstance().playAudioMessage(msg.getMessage());
//        }

        if(!msg.getMessage().isFromUser()) {
            mFragment.switchToDetail(new String(msg.getMessage().msgData));
        }

    }
}

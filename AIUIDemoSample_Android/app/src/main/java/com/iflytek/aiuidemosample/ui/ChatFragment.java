package com.iflytek.aiuidemosample.ui;

import android.Manifest;
import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageView;

import com.iflytek.aiuidemosample.R;
import com.iflytek.aiuidemosample.databinding.ChatFragmentBinding;
import com.iflytek.aiuidemosample.model.Message;
import com.iflytek.aiuidemosample.repository.MessagesCenterRepository;
import com.iflytek.aiuidemosample.ui.common.InputState;
import com.iflytek.aiuidemosample.ui.widget.PopupWindowFactory;
import com.iflytek.aiuidemosample.viewmodel.MessagesViewModel;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.record.PcmRecorder;
import com.iflytek.cloud.util.VolumeUtil;
import com.karumi.dexter.Dexter;
import com.karumi.dexter.PermissionToken;
import com.karumi.dexter.listener.PermissionDeniedResponse;
import com.karumi.dexter.listener.PermissionGrantedResponse;
import com.karumi.dexter.listener.PermissionRequest;
import com.karumi.dexter.listener.single.BasePermissionListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.List;

import io.reactivex.functions.Consumer;

/**
 * Created by PR on 2017/7/26.
 */

public class ChatFragment extends Fragment implements PcmRecorder.PcmRecordListener{
    private MessagesViewModel mMessageModel;
    private List<Message> mCurrentMsgList;
    private PcmRecorder mRecorder;

    private MessageListAdapter mMsgAdapter;

    private ChatFragmentBinding mChatBinding;
    private PopupWindowFactory mVoicePop;
    private ImageView VolumeView;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        mChatBinding = DataBindingUtil.inflate(inflater, R.layout.chat_fragment,
                container, false);
        return mChatBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        try {
            MessagesViewModel.init(getActivity(), new MessagesCenterRepository(getActivity(), new JSONObject(readAIUICfg())));
            mMessageModel = MessagesViewModel.getInstance();
            initChatView();
            initSendAction();
            initVoiceAction();
            setInputState(InputState.TEXT_INPUT);

            Dexter.withActivity(getActivity())
                    .withPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    .withListener(new BasePermissionListener(){

                        @Override
                        public void onPermissionGranted(PermissionGrantedResponse response) {
                            super.onPermissionGranted(response);
                        }

                        @Override
                        public void onPermissionRationaleShouldBeShown(PermissionRequest permission, PermissionToken token) {
                            super.onPermissionRationaleShouldBeShown(permission, token);
                            token.continuePermissionRequest();
                        }

                        @Override
                        public void onPermissionDenied(PermissionDeniedResponse response) {
                            super.onPermissionDenied(response);
                            mMessageModel.fakeAIUIResult(0, "permission", "请重启应用允许请求的权限");
                        }
                    })
                    .check();

            mMessageModel.checkContactUpdate();
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        mMessageModel.checkContactUpdate();
    }

    private void initVoiceAction() {
        mChatBinding.emotionVoice.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                setInputState(mChatBinding.getState()==InputState.VOICE_INPUT?
                        InputState.TEXT_INPUT:InputState.VOICE_INPUT);
                dismissKeyboard(view.getWindowToken());
            }
        });

        View view = View.inflate(getActivity(), R.layout.layout_microphone, null);
        VolumeView = (ImageView) view.findViewById(R.id.iv_recording_icon);
        mVoicePop = new PopupWindowFactory(getActivity(), view);

        mChatBinding.voiceText.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(final View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        mChatBinding.voiceText.setPressed(true);
                        Dexter
                                .withActivity(getActivity())
                                .withPermission(Manifest.permission.RECORD_AUDIO)
                                .withListener(new BasePermissionListener(){
                                    @Override
                                    public void onPermissionGranted(PermissionGrantedResponse response) {
                                        super.onPermissionGranted(response);
                                        if(mChatBinding.voiceText.isPressed()){
                                            mVoicePop.showAtLocation(v, Gravity.CENTER, 0, 0);
                                            setInputState(InputState.VOICE_INPUTING);
                                            try {
                                                mRecorder = new PcmRecorder(16000, 40);
                                                mMessageModel.startWriteAudio();
                                                mRecorder.startRecording(ChatFragment.this);
                                            } catch (SpeechError speechError) {
                                                speechError.printStackTrace();
                                            }
                                        }

                                    }

                                    @Override
                                    public void onPermissionRationaleShouldBeShown(PermissionRequest permission, PermissionToken token) {
                                        super.onPermissionRationaleShouldBeShown(permission, token);
                                        token.continuePermissionRequest();
                                    }
                                }).check();
                        break;
                    case MotionEvent.ACTION_UP:
                        mChatBinding.voiceText.setPressed(false);
                        mVoicePop.dismiss();
                        setInputState(InputState.TEXT_INPUT);
                        mMessageModel.stopWriteAudio();
                        if(mRecorder != null){
                            mRecorder.stopRecord(true);
                            mRecorder = null;
                        }

                        break;
                }
                return true;
            }
        });
    }

    private void setInputState(InputState state){
        mChatBinding.setState(state);
        mChatBinding.executePendingBindings();
    }

    private void initSendAction(){
        mChatBinding.emotionSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                doSend(view);
            }
        });

        mChatBinding.emotionSend.setOnKeyListener(new View.OnKeyListener() {
            @Override
            public boolean onKey(View view, int keyCode, KeyEvent keyEvent) {
                if ((keyEvent.getAction() == KeyEvent.ACTION_DOWN)
                        && (keyCode == KeyEvent.KEYCODE_ENTER)){
                    doSend(view);
                    return true;
                }
                return false;
            }
        });
    }

    private void doSend(View view) {
        String msg = mChatBinding.editText.getText().toString();
        if(!TextUtils.isEmpty(msg)){
            mMessageModel.sendText(msg);
            mChatBinding.editText.setText("");
        }
    }

    private void initChatView(){
        LinearLayoutManager layout = new LinearLayoutManager(getActivity());
        layout.setStackFromEnd(true);
        mChatBinding.chatList.setLayoutManager(layout);
//        mChatBinding.chatList.setItemAnimator(new OvershootInLeftAnimator());
        mMsgAdapter = new MessageListAdapter();
        mChatBinding.chatList.setAdapter(mMsgAdapter);
        mChatBinding.chatList.setVerticalScrollBarEnabled(true);
        mMsgAdapter.registerAdapterDataObserver(new RecyclerView.AdapterDataObserver() {
            @Override
            public void onItemRangeInserted(int positionStart, int itemCount) {
                super.onItemRangeInserted(positionStart, itemCount);
                mChatBinding.chatList.scrollToPosition(mCurrentMsgList.size() - 1);
            }
        });
        //获取交互消息，更新展示
        mMessageModel.getInteractMessages().subscribe(new Consumer<List<Message>>() {
            @Override
            public void accept(List<Message> messages) throws Exception {
                mCurrentMsgList = messages;
                mMsgAdapter.replace(messages);
                mChatBinding.executePendingBindings();
            }
        });
    }

    private void dismissKeyboard(IBinder windowToken) {
        Activity activity = getActivity();
        if (activity != null) {
            InputMethodManager imm = (InputMethodManager) activity.getSystemService(
                    Context.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(windowToken, 0);
        }
    }

    private String readAIUICfg(){
        try {
            InputStream input = getActivity().getAssets().open("aiui_phone.cfg");
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            int read = 0;
            while((read=input.read(buffer)) != -1){
                output.write(buffer, 0, read);
            }

            input.close();
            output.close();

            return new String(output.toByteArray());
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public void onRecordBuffer(byte[] bytes, int offset, int length) {
        final byte[] audio = Arrays.copyOfRange(bytes, offset, length);
        mMessageModel.writeAudio(audio);


        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                int i = VolumeUtil.computeVolume(audio, audio.length);
                int level = 5000 + 8000 * i / 100;

                if(VolumeView.getDrawable().setLevel(level)){
                    VolumeView.getDrawable().invalidateSelf();
                }
            }
        });
    }

    @Override
    public void onError(SpeechError speechError) {

    }

    @Override
    public void onRecordStarted(boolean b) {

    }

    @Override
    public void onRecordReleased() {

    }
}

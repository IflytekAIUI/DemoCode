package com.iflytek.aiuidemosample.ui;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

import com.iflytek.aiuidemosample.model.Message;

import io.reactivex.Completable;
import io.reactivex.functions.Action;
import io.reactivex.schedulers.Schedulers;

/**
 * Created by PR on 2017/8/3.
 */

public class AudioHandler {
    private static AudioHandler sInstance;
    public static AudioHandler getsInstance(){
        if(sInstance == null){
            sInstance = new AudioHandler();
        }

        return sInstance;
    }


    private AudioTrack mTrack;

    private AudioHandler(){
        mTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                16000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT,
                AudioTrack.getMinBufferSize(16000, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT),
                AudioTrack.MODE_STREAM);
    }

    public void playAudioMessage(final Message message){
        if(message.msgType == Message.MsgType.Voice){
            Completable.complete().observeOn(Schedulers.io())
                    .subscribe(new Action() {
                        @Override
                        public void run() throws Exception {
                            mTrack.stop();
                            mTrack.play();
                            mTrack.write(message.msgData,0, message.msgData.length);
                        }
                    });

        }
    }
}

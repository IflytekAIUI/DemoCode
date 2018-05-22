package com.iflytek.aiui.demo.chat.repository.player;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.MutableLiveData;
import android.content.Context;
import android.net.Uri;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.ControlDispatcher;
import com.google.android.exoplayer2.DefaultControlDispatcher;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.Player;
import com.google.android.exoplayer2.extractor.DefaultExtractorsFactory;
import com.google.android.exoplayer2.extractor.ExtractorsFactory;
import com.google.android.exoplayer2.source.DynamicConcatenatingMediaSource;
import com.google.android.exoplayer2.source.ExtractorMediaSource;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.TrackGroupArray;
import com.google.android.exoplayer2.trackselection.TrackSelectionArray;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultBandwidthMeter;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.util.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by PR on 2017/12/21.
 */

public class AIUIPlayer {

    public static class SongInfo {
        public String author;
        public String songName;
        public String audioPath;

        public SongInfo(String author, String songName, String audioPath) {
            this.author = author;
            this.songName = songName;
            this.audioPath = audioPath;
        }
    }

    private List<SongInfo> songList = new ArrayList<>();
    private ExoPlayer mPlayer;
    private Context mContext;
    private ControlDispatcher mDispatcher;
    private boolean mActive = false;
    private MutableLiveData<PlayState> mState = new MutableLiveData<>();
    private int mCurrentIndex = -1;

    public AIUIPlayer(Context context, ExoPlayer player) {
        this.mPlayer = player;
        this.mContext = context;
        this.mDispatcher = new DefaultControlDispatcher();

        this.mPlayer.addListener(new Player.DefaultEventListener() {
            // getCurrentWindowIndex 返回不准问题（https://github.com/google/ExoPlayer/issues/2799）
            @Override
            public void onPositionDiscontinuity(int reason) {
                super.onPositionDiscontinuity(reason);
                mCurrentIndex = mPlayer.getCurrentWindowIndex();
                if(mCurrentIndex > -1 && mCurrentIndex < songList.size()) {
                    SongInfo info = songList.get(mCurrentIndex);
                    mState.postValue(new PlayState(mActive, true, info.songName));
                }
            }

            @Override
            public void onTracksChanged(TrackGroupArray trackGroups, TrackSelectionArray trackSelections) {
                super.onTracksChanged(trackGroups, trackSelections);
                mCurrentIndex = mPlayer.getCurrentWindowIndex();
                if(mCurrentIndex > -1 && mCurrentIndex < songList.size()) {
                    SongInfo info = songList.get(mCurrentIndex);
                    mState.postValue(new PlayState(mActive, true, info.songName));
                }
            }

            @Override
            public void onPlayerStateChanged(boolean playWhenReady, int playbackState) {
                super.onPlayerStateChanged(playWhenReady, playbackState);
                if(playWhenReady) {
                    mActive = true;
                }
                if(mCurrentIndex > -1 && mCurrentIndex < songList.size()) {
                    SongInfo info = songList.get(mCurrentIndex);
                    mState.postValue(new PlayState(mActive, playWhenReady, info.songName));
                }
            }
        });
    }

    public void playList(List<SongInfo> list) {
        songList = list;
        if(songList == null) {
            songList = new ArrayList<>();
        }
        DynamicConcatenatingMediaSource source = new DynamicConcatenatingMediaSource();
        for (SongInfo info : list) {
            DefaultBandwidthMeter bandwidthMeter = new DefaultBandwidthMeter();
            // Produces DataSource instances through which media data is loaded.
            DataSource.Factory dataSourceFactory = new DefaultDataSourceFactory(mContext,
                    Util.getUserAgent(mContext, "yourApplicationName"), bandwidthMeter);
            ExtractorsFactory extractorsFactory = new DefaultExtractorsFactory();
            Uri uri = Uri.parse(info.audioPath);
            MediaSource mediaSource = new ExtractorMediaSource(uri, dataSourceFactory, extractorsFactory, null, null);

            source.addMediaSource(mediaSource);
        }

        mPlayer.prepare(source);
        mPlayer.setPlayWhenReady(true);
    }

    public void pause() {
        mDispatcher.dispatchSetPlayWhenReady(mPlayer, false);
    }

    public void play() {
        mDispatcher.dispatchSetPlayWhenReady(mPlayer, true);
    }

    public void next() {
        int index;
        if (mPlayer.getNextWindowIndex() != C.INDEX_UNSET) {
            index = mPlayer.getNextWindowIndex();
        } else {
            index = mPlayer.getCurrentWindowIndex();
        }
        mDispatcher.dispatchSeekTo(mPlayer, index, C.TIME_UNSET);
        play();
    }

    public void prev() {
        int index;
        if (mPlayer.getPreviousWindowIndex() != C.INDEX_UNSET) {
            index = mPlayer.getPreviousWindowIndex();
        } else {
            index = mPlayer.getCurrentWindowIndex();
        }
        mDispatcher.dispatchSeekTo(mPlayer, index, C.TIME_UNSET);
        play();
    }

    public void stop() {
        mActive = false;
        pause();
    }

    public LiveData<PlayState> getLiveState() {
        return mState;
    }
}

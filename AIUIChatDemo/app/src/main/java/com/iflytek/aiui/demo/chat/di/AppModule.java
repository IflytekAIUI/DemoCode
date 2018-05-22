package com.iflytek.aiui.demo.chat.di;

import android.app.Application;
import android.arch.persistence.room.Room;
import android.content.Context;

import com.google.android.exoplayer2.ExoPlayerFactory;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.trackselection.AdaptiveTrackSelection;
import com.google.android.exoplayer2.trackselection.DefaultTrackSelector;
import com.google.android.exoplayer2.trackselection.TrackSelection;
import com.google.android.exoplayer2.trackselection.TrackSelector;
import com.google.android.exoplayer2.upstream.BandwidthMeter;
import com.google.android.exoplayer2.upstream.DefaultBandwidthMeter;
import com.iflytek.aiui.demo.chat.db.MessageDB;
import com.iflytek.aiui.demo.chat.db.MessageDao;
import com.iflytek.aiui.demo.chat.repository.AIUIRepository;
import com.iflytek.aiui.demo.chat.repository.SettingsRepo;
import com.iflytek.aiui.demo.chat.repository.Storage;
import com.iflytek.aiui.demo.chat.repository.player.AIUIPlayer;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import javax.inject.Named;
import javax.inject.Singleton;

import dagger.Module;
import dagger.Provides;

/**
 * Created by PR on 2017/11/6.
 */

@Module(includes = ViewModelModule.class)
public class AppModule {
    @Provides
    @Singleton
    public Context providesContext(Application application) {
        return application;
    }

    @Named("AIUI cfg")
    @Provides
    @Singleton
    public JSONObject provideAIUICfg(Storage storage) {
        try {
            JSONObject config = new JSONObject(storage.readAssetFile("cfg/aiui_phone.cfg"));
            return config;
        } catch (JSONException e) {
            e.printStackTrace();
            return null;
        }
    }

    @Provides
    @Singleton
    public AIUIRepository provideCenterRepository(Application application
            , @Named("AIUI cfg") JSONObject config, MessageDao dao, SettingsRepo settings) {
        SpeechCreateIfExists(application, config);
        return new AIUIRepository(application, config, dao, settings);
    }

    private void SpeechCreateIfExists(Application application, JSONObject config) {
        try {
            Class UtilityClass = getClass().getClassLoader().loadClass("com.iflytek.cloud.SpeechUtility");
            Method createMethod = UtilityClass.getDeclaredMethod("createUtility", Context.class, String.class);
            createMethod.invoke(null, application, "appid="
                + config.optJSONObject("login").optString("appid") + ",engine_start=ivw" );
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
    }


    @Provides
    @Singleton
    public AIUIPlayer providePlayer(Application application){
        // 1. Create a default TrackSelector
        BandwidthMeter bandwidthMeter = new DefaultBandwidthMeter();
        TrackSelection.Factory videoTrackSelectionFactory =
                new AdaptiveTrackSelection.Factory(bandwidthMeter);
        TrackSelector trackSelector =
                new DefaultTrackSelector(videoTrackSelectionFactory);

        // 2. Create the player
        SimpleExoPlayer player =
                ExoPlayerFactory.newSimpleInstance(application, trackSelector);

        return new AIUIPlayer(application, player);
    }

    @Provides
    @Singleton
    public MessageDB provideMessageDB(Application application) {
        return Room.inMemoryDatabaseBuilder(application, MessageDB.class).build();
    }

    @Provides
    @Singleton
    public MessageDao provideMessageDao(MessageDB db) {
        return db.messageDao();
    }
}

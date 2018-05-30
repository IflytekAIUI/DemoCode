package com.iflytek.aiui.demo.chat.repository;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.MutableLiveData;
import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.iflytek.aiui.demo.chat.BuildConfig;
import com.iflytek.aiui.demo.chat.model.Settings;
import org.json.JSONObject;
import javax.inject.Inject;
import javax.inject.Named;
import javax.inject.Singleton;

/**
 * Created by PR on 2018/1/18.
 */

@Singleton
public class SettingsRepo {
    public static final String KEY_AIUI_WAKEUP = "aiui_wakeup";
    public static final String KEY_LAST_APPID = "last_appid";
    public static final String KEY_LAST_KEY = "last_key";
    public static final String KEY_LAST_SCENE = "last_scene";
    public static final String KEY_APPID = "appid";
    public static final String KEY_APPIDKEY = "key";
    public static final String KEY_SCENE = "scene";

    private Context mContext;
    private String mDefaultAppid;
    private String mDefaultAppKey;
    private String mDefaultScene;
    private MutableLiveData<Settings> mSettings = new MutableLiveData<>();
    private MutableLiveData<Boolean> mLiveWakeUpEnable = new MutableLiveData<>();

    @Inject
    public SettingsRepo(Context context, @Named("AIUI cfg") JSONObject config) {
        mContext = context;
        mDefaultAppid = config.optJSONObject("login").optString(KEY_APPID);
        mDefaultAppKey = config.optJSONObject("login").optString(KEY_APPIDKEY);
        mDefaultScene = config.optJSONObject("global").optString(KEY_SCENE);

        mSettings.postValue(getLatestSettings(mContext));
    }

    public void config(String appid, String key, String scene) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(mContext);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(KEY_APPID, appid);
        editor.putString(KEY_APPIDKEY, key);
        editor.putString(KEY_SCENE, scene);
        editor.commit();

        updateSettings();
    }

    public LiveData<Settings> getSettings() {
        return mSettings;
    }

    public void updateSettings() {
        mSettings.postValue(getLatestSettings(mContext));
    }

    public LiveData<Boolean> getWakeUpEnableState() {
        return mLiveWakeUpEnable;
    }

    @NonNull
    private Settings getLatestSettings(Context context) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        String lastConfigAppid = preferences.getString(KEY_LAST_APPID, "");
        String lastConfigKey = preferences.getString(KEY_LAST_KEY, "");
        String lastConfigScene = preferences.getString(KEY_LAST_SCENE, "");
        //不同说明重装更新了assets下的配置文件，同步所有到所有Appid设置
        if(!lastConfigAppid.equals(mDefaultAppid) || !lastConfigKey.equals(mDefaultAppKey) ||
                lastConfigScene.equals(mDefaultScene)) {
            syncLastConfig(preferences);
            syncConfig(preferences);
        }

        //appid和key为空时恢复默认
        if(TextUtils.isEmpty(preferences.getString(KEY_APPID, "")) && TextUtils.isEmpty(
                preferences.getString(KEY_APPIDKEY, ""))) {
            syncConfig(preferences);
        }

        //appid不为默认配置时禁止唤醒功能开启
        if(!mDefaultAppid.equals(preferences.getString(KEY_APPID, ""))) {
            SharedPreferences.Editor editor = preferences.edit();
            editor.putBoolean(KEY_AIUI_WAKEUP, false);
            editor.commit();
            mLiveWakeUpEnable.postValue(false);
        } else {
            mLiveWakeUpEnable.postValue(BuildConfig.WAKEUP_ENABLE);
        }

        Settings settings = new Settings();
        settings.wakeup = preferences.getBoolean(KEY_AIUI_WAKEUP, false);
        settings.bos = Integer.valueOf(preferences.getString("aiui_bos", "5000"));
        settings.eos = Integer.valueOf(preferences.getString("aiui_eos", "1000"));
        settings.debugLog = preferences.getBoolean("aiui_debug_log", true);
        settings.saveDebugLog = preferences.getBoolean("aiui_save_datalog", false);
        settings.appid = preferences.getString(KEY_APPID, "");
        settings.key = preferences.getString(KEY_APPIDKEY, "");
        settings.scene = preferences.getString(KEY_SCENE, "");
        return settings;
    }

    private void syncLastConfig(SharedPreferences preferences) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(KEY_LAST_APPID, mDefaultAppid);
        editor.putString(KEY_LAST_KEY, mDefaultAppKey);
        editor.putString(KEY_LAST_SCENE, mDefaultScene);
        editor.commit();
    }

    private void syncConfig(SharedPreferences preferences) {
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(KEY_APPID, mDefaultAppid);
        editor.putString(KEY_APPIDKEY, mDefaultAppKey);
        editor.putString(KEY_SCENE, mDefaultScene);
        editor.commit();
    }
}

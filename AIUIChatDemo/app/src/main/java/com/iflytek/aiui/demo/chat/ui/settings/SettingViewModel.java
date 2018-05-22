package com.iflytek.aiui.demo.chat.ui.settings;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.ViewModel;

import com.iflytek.aiui.demo.chat.repository.SettingsRepo;

import javax.inject.Inject;

/**
 * Created by PR on 2017/12/14.
 */

public class SettingViewModel extends ViewModel {
    private SettingsRepo mSettingsRepo;
    @Inject
    public SettingViewModel(SettingsRepo settingsRepo) {
        mSettingsRepo = settingsRepo;
    }

    public void syncLastSetting() {
        mSettingsRepo.updateSettings();
    }

    public LiveData<Boolean> isWakeUpAvailable() {
        return mSettingsRepo.getWakeUpEnableState();
    }
}


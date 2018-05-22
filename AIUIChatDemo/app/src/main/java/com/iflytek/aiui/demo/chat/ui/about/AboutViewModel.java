package com.iflytek.aiui.demo.chat.ui.about;

import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.ViewModel;

import com.iflytek.aiui.demo.chat.repository.AIUIRepository;

import javax.inject.Inject;

/**
 * Created by PR on 2018/1/12.
 */

public class AboutViewModel extends ViewModel {
    private AIUIRepository mAIUI;

    @Inject
    public AboutViewModel(AIUIRepository aiuiRepository) {
        mAIUI = aiuiRepository;
    }

    public LiveData<String> getUID() {
        return mAIUI.getUID();
    }
}

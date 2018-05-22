package com.iflytek.aiui.demo.chat.ui.settings;

import android.arch.lifecycle.Observer;
import android.arch.lifecycle.ViewModelProvider;
import android.arch.lifecycle.ViewModelProviders;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.design.widget.Snackbar;
import android.support.v7.preference.EditTextPreference;
import android.support.v7.preference.PreferenceFragmentCompat;
import android.support.v7.preference.SwitchPreferenceCompat;

import com.iflytek.aiui.demo.chat.R;

import javax.inject.Inject;

import dagger.android.support.AndroidSupportInjection;

/**
 * Created by PR on 2017/12/12.
 */

public class SettingsFragment extends PreferenceFragmentCompat implements SharedPreferences.OnSharedPreferenceChangeListener {
    public static final String AIUI_EOS = "aiui_eos";
    public static final String AIUI_WAKEUP = "aiui_wakeup";
    @Inject
    ViewModelProvider.Factory mViewModelFactory;
    private SettingViewModel mSettingModel;
    private EditTextPreference eosPreference;
    private SwitchPreferenceCompat wakeupPreference;

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        AndroidSupportInjection.inject(this);
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.pref_settings);
        eosPreference = (EditTextPreference) (getPreferenceManager().findPreference(AIUI_EOS));
        eosPreference.setSummary(String.format("%sms", getPreferenceManager().getSharedPreferences().getString(AIUI_EOS, "1000")));
        wakeupPreference = (SwitchPreferenceCompat) getPreferenceManager().findPreference(AIUI_WAKEUP);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mSettingModel = ViewModelProviders.of(this, mViewModelFactory).get(SettingViewModel.class);

        mSettingModel.isWakeUpAvailable().observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(@Nullable Boolean enable) {
                wakeupPreference.setEnabled(enable);
            }
        });
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String s) {
        if(AIUI_EOS.equals(s)){
           String eos = sharedPreferences.getString(s, "1000");
           if(!isNumeric(eos)) {
               eosPreference.setText("1000");
               Snackbar.make(getView(), R.string.eos_invalid_tip , Snackbar.LENGTH_LONG).show();
           } else {
               eosPreference.setSummary(String.format("%sms", eos));
           }
        }
    }


    private boolean isNumeric(String str) {
        try {
            Integer.valueOf(str);
            return true;
        } catch (Exception e) {
            return false;
        }
    }



    @Override
    public void onResume() {
        super.onResume();
        getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onStop() {
        super.onStop();

         mSettingModel.syncLastSetting();
    }


}

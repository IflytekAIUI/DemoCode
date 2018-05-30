package com.iflytek.aiui.demo.chat.di;

import com.iflytek.aiui.demo.chat.ChatActivity;

import dagger.Module;
import dagger.android.ContributesAndroidInjector;

/**
 * Created by PR on 2017/11/9.
 */

@Module
public abstract class ChatActivityModule {
    @ContributesAndroidInjector(modules = {FragmentBuildersModule.class})
    public abstract ChatActivity contributesChatActivity();
}

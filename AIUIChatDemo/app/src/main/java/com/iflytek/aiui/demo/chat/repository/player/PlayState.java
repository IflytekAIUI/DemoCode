package com.iflytek.aiui.demo.chat.repository.player;

/**
 * Created by PR on 2018/2/1.
 */
public class PlayState {
    public PlayState(boolean active, boolean playing, String info) {
        this.active = active;
        this.playing = playing;
        this.info = info;
    }

    public boolean active;
    public boolean playing;
    public String info;
}

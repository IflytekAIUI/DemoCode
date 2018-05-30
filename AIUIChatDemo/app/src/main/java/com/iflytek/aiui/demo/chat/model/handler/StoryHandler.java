package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.repository.player.AIUIPlayer;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by PR on 2017/12/21.
 */

public class StoryHandler extends Handler {

    public StoryHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        if(result.data != null) {
            List<AIUIPlayer.SongInfo> songList = new ArrayList<>();
            JSONArray list = result.data.optJSONArray("result");
            if(list != null){
                for(int index = 0; index < list.length(); index++){
                    JSONObject audio = list.optJSONObject(index);
                    String audioPath = audio.optString("playUrl");
                    String songname = audio.optString("name");
                    String author = audio.optString("author");

                    songList.add(new AIUIPlayer.SongInfo(author, songname, audioPath));
                }

            }

            if(songList.size() != 0) {
                mPlayer.playList(songList);
                if(isNeedShowContrlTip()) {
                    result.answer = result.answer + NEWLINE_NO_HTML + NEWLINE_NO_HTML + CONTROL_TIP;
                }
            }
        }
        return result.answer;

    }
}


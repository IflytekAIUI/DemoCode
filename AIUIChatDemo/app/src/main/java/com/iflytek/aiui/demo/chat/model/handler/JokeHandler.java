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

public class JokeHandler extends Handler {
    public JokeHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        if(result.data != null) {
            List<AIUIPlayer.SongInfo> songList = new ArrayList<>();
            JSONArray list = result.data.optJSONArray("result");
            if(list != null){
                for(int index = 0; index < list.length(); index++){
                    JSONObject item = list.optJSONObject(index);
                    // 音频笑话
                    if("1".equals(item.optString("type"))) {
                        String audioPath = item.optString("mp3Url");
                        String songname = item.optString("title");
                        String author = item.optString("author");

                        songList.add(new AIUIPlayer.SongInfo(author, songname, audioPath));
                    } else {
                       // 文本笑话(显示第一条结果)
                        result.answer += NEWLINE_NO_HTML + NEWLINE_NO_HTML + item.optString("content");
                        break;
                    }
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


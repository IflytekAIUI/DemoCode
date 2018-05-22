package com.iflytek.aiui.demo.chat.model.handler;

import android.text.TextUtils;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.repository.player.AIUIPlayer;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

/**
 * Created by PR on 2017/8/4.
 */

public class DefaultHandler extends Handler {
    private Pattern typePattern = Pattern.compile("\\d");

    public DefaultHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
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
                    int contentType = item.optInt("type", -1);
                    switch (contentType) {
                        //文本内容
                        case 0:{
                            //显示第一条结果
                            result.answer += NEWLINE_NO_HTML + NEWLINE_NO_HTML + item.optString("content");
                            break;
                        }
                        //音频内容(1) 视频内容(2)
                        case 1:
                        case 2: {
                            String audioPath = item.optString("url");
                            String songname = item.optString("title");
                            if(TextUtils.isEmpty(songname)) {
                                songname = item.optString("name");
                            }

                            songList.add(new AIUIPlayer.SongInfo(songname, songname, audioPath));
                            break;
                        }
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

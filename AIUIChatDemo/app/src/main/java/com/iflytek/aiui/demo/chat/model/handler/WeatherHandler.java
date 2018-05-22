package com.iflytek.aiui.demo.chat.model.handler;

import android.Manifest;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import org.json.JSONArray;
import org.json.JSONObject;

/**
 * Created by PR on 2018/1/30.
 */

public class WeatherHandler extends Handler {
    private static boolean notified = false;
    public WeatherHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }

    @Override
    public String getFormatContent() {
        if(notified) {
           return result.answer;
        } else {
            StringBuilder answer = new StringBuilder(result.answer);
            JSONArray slots = result.semantic.optJSONArray("slots");
            for(int index = 0; index < slots.length(); index++) {
                JSONObject item = slots.optJSONObject(index);
                if(item.optString("name").contains("location")){
                    if(item.optString("value").contains("CURRENT_CITY")) {
                        answer.append(NEWLINE);
                        answer.append(NEWLINE);
                        answer.append("<a href=\"use_loc\">使用定位让天气信息更准确</a>");
                        notified = true;
                        break;
                    }
                }
            }
            return answer.toString();
        }
    }

    @Override
    public boolean urlClicked(String url) {
        if("use_loc".equals(url)){
            mPermissionChecker.checkPermission(Manifest.permission.ACCESS_FINE_LOCATION, new Runnable() {
                @Override
                public void run() {
                    mMessageViewModel.useLocationData();
                }
            }, null);
        }

        return true;
    }
}

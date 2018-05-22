package com.iflytek.aiui.demo.chat.model.handler;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.model.data.SpeakableSyncData;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by PR on 2017/8/7.
 */

public class DishSkillHandler extends Handler{
    private static Map<String, JSONObject> dishDetailCache = new HashMap<>();

    public DishSkillHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }


    @Override
    public String getFormatContent() {
        String intent = result.semantic.optString("intent");
        if(intent.equals("DishSearchIntent")) {
            StringBuilder dishesDisplay = new StringBuilder();
            dishesDisplay.append(result.answer);
            dishesDisplay.append(NEWLINE);
            dishesDisplay.append(NEWLINE);
            JSONArray dishes = result.data.optJSONArray("result");
            for(int index=0;index<dishes.length();index++){
                JSONObject dish = dishes.optJSONObject(index);
                dishesDisplay.append(String.format("%d. %s", index + 1, dish.optString("title")));
                dishesDisplay.append(NEWLINE);
                dishDetailCache.put(dish.optString("title"), dish);
            }
            dishesDisplay.append(NEWLINE);

            syncDishData();
            return dishesDisplay.toString();
        }else if(intent.equals("DishStepIntent")){
            JSONObject slot = result.semantic.optJSONArray("slots").optJSONObject(0);
            StringBuilder detailDisplay = new StringBuilder();
            String dishName = slot.optString("value");
            JSONObject dishDetail = dishDetailCache.get(dishName);
            if(dishDetail != null){
                detailDisplay.append(String.format("%s 是这么做滴", dishName));
                detailDisplay.append(NEWLINE);
                detailDisplay.append(NEWLINE);
                detailDisplay.append(String.format("<img src=\"%s\"/>",
                        "http://pic3.qqmofasi.com/2014/04/17/23_f5t5ttOr6KY4rQtdqMY6_large.jpg"));
                detailDisplay.append(NEWLINE);
                detailDisplay.append(NEWLINE);
                detailDisplay.append(String.format("配料: %s%s", NEWLINE, dishDetail.optString("accessory")));
                detailDisplay.append(NEWLINE);
                detailDisplay.append(NEWLINE);
                detailDisplay.append(String.format("原料: %s%s", NEWLINE, dishDetail.optString("ingredient")));
                detailDisplay.append(NEWLINE);
                detailDisplay.append(NEWLINE);
                detailDisplay.append(String.format("步骤: %s",
                        dishDetail.optString("steps")
                                .replaceAll("(\\d+\\.)",  NEWLINE + "$1")).replaceAll(";", ""));
            }else {
                detailDisplay.append(String.format("没有为您找到%s对应做法", dishName));
            }

            return detailDisplay.toString();
        }
        return result.answer;
    }

    private void syncDishData() {
        StringBuilder speakableData = new StringBuilder();
        JSONArray dishes = result.data.optJSONArray("result");
        for(int index=0;index<dishes.length();index++){
            JSONObject dish = dishes.optJSONObject(index);

            speakableData.append(String.format("{\"title\":\"%s\"}\n", dish.optString("title")));
        }

        SpeakableSyncData speakableSyncData = new SpeakableSyncData(
                "FOOBAR.dishRes", "FOOBAR.DishSkill", speakableData.toString());

        mMessageViewModel.syncSpeakableData(speakableSyncData);
        mMessageViewModel.putPersParam("uid", "");
        mMessageViewModel.fakeAIUIResult(0,
                "FOOBAR.DishSkill", "同步成功后通过 xxx怎么做 查询具体做法");
    }
}

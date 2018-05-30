package com.iflytek.aiui.demo.chat.model.handler;


import com.iflytek.aiui.demo.chat.model.data.DynamicEntityData;
import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * Created by PR on 2017/8/8.
 */

public class OrderMenuHandler extends Handler {
    public static List<String> orders = new ArrayList<>();

    public OrderMenuHandler(ChatViewModel model, PlayerViewModel player, PermissionChecker checker, SemanticResult result) {
        super(model, player, checker, result);
    }


    @Override
    public String getFormatContent() {
        String intent = result.semantic.optString("intent");
        if(intent.equals("wantOrderIntent")){
            orders.clear();
            StringBuilder orderResponse = new StringBuilder();
            orderResponse.append(String.format("请选择您所在的地区分店"));
            orderResponse.append(NEWLINE);
            orderResponse.append(NEWLINE);
            orderResponse.append(NEWLINE);
            orderResponse.append(String.format("<a href=\"%s\">饭真香001分店</a>", "001"));
            orderResponse.append(NEWLINE);
            orderResponse.append(NEWLINE);
            orderResponse.append(String.format("<a href=\"%s\">饭真香002分店</a>", "002"));

            return orderResponse.toString();
        }else if(intent.equals("orderItemIntent")){
            String item = result.semantic.optJSONArray("slots").
                    optJSONObject(0).optString("value");
            orders.add(item);
            StringBuilder currentOrders = new StringBuilder();
            currentOrders.append("好的，当前您已经点了如下菜品：");
            currentOrders.append(NEWLINE);
            currentOrders.append(NEWLINE);
            currentOrders.append(NEWLINE);
            for(int index=0;index<orders.size();index++){
                currentOrders.append(String.format("%d. %s", index + 1, orders.get(index)));
                currentOrders.append(NEWLINE);
            }

            return currentOrders.toString();
        } else if(intent.equals("finishOrderIntent")){
            StringBuilder finishOrder = new StringBuilder();
            int priceSum = 0;
            finishOrder.append("好的，马上为您准备上菜，当前包含如下菜品：");
            finishOrder.append(NEWLINE);
            finishOrder.append(NEWLINE);
            finishOrder.append(NEWLINE);
            for(int index=0;index<orders.size();index++){
                int price = new Random().nextInt(20) + 20;
                finishOrder.append(String.format("%d. %s %d元",
                        index + 1,
                        orders.get(index),
                        price));
                priceSum += price;
                finishOrder.append(NEWLINE);
            }
            finishOrder.append(NEWLINE);
            finishOrder.append(NEWLINE);
            finishOrder.append(String.format("<strong>总价 %d元，请马上结账，我们不支持打白条或霸王餐</strong>"
                    , priceSum));
            return finishOrder.toString();
        }
        return result.answer;
    }

    @Override
    public boolean urlClicked(String url) {
        String menuSrcData = "";
        if(url.equals("001")){
            menuSrcData = mMessageViewModel.readAssetFile("data/001_branch_menu.txt");
        }else if(url.equals("002")){
            menuSrcData = mMessageViewModel.readAssetFile("data/002_branch_menu.txt");
        }

        String[] menuItemsStr = menuSrcData.split("\r?\n");
        StringBuffer menuItemData = new StringBuffer();
        for(String item : menuItemsStr){
            menuItemData.append(String.format("{\"name\": \"%s\"}\n", item));
        }

        mMessageViewModel.syncDynamicData(new DynamicEntityData(
                "FOOBAR.menuRes", "branch", url, menuItemData.toString()
        ));
        mMessageViewModel.putPersParam("branch", url);

        //构造欢迎消息放入到消息队列中
        StringBuffer branchMenu = new StringBuffer();
        branchMenu.append(String.format("您好，%s分店菜单如下", url));
        branchMenu.append(NEWLINE);
        branchMenu.append(NEWLINE);
        for(int index=0;index<menuItemsStr.length;index+=2){
            branchMenu.append(String.format("<p>%-5s|%-5s</p>",
                    menuItemsStr[index],
                    index+1>=menuItemsStr.length? "":menuItemsStr[index+1], 5));
        }
        branchMenu.append(NEWLINE);
        branchMenu.append(NEWLINE);
        branchMenu.append("可以按如下说法进行点菜：");
        branchMenu.append(NEWLINE);
        branchMenu.append("我要点xxx");
        branchMenu.append(NEWLINE);
        branchMenu.append("再点个xxx");
        branchMenu.append(NEWLINE);
        branchMenu.append("点个xxx");
        branchMenu.append(NEWLINE);
        branchMenu.append(NEWLINE);
        branchMenu.append(NEWLINE);
        branchMenu.append("可以按如下说法结束点菜：");
        branchMenu.append(NEWLINE);
        branchMenu.append("就这些");
        branchMenu.append(NEWLINE);
        branchMenu.append("好了，就这些");
        branchMenu.append(NEWLINE);
        branchMenu.append("可以了，上菜吧");
        branchMenu.append(NEWLINE);


        mMessageViewModel.fakeAIUIResult(0, "FOOBAR.MenuSkill", branchMenu.toString());
        return true;
    }
}

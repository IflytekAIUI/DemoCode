package com.iflytek.aiui.demo.chat.model;

import com.iflytek.aiui.demo.chat.model.data.SemanticResult;
import com.iflytek.aiui.demo.chat.model.handler.DefaultHandler;
import com.iflytek.aiui.demo.chat.model.handler.DishSkillHandler;
import com.iflytek.aiui.demo.chat.model.handler.DynamicEntityHandler;
import com.iflytek.aiui.demo.chat.model.handler.Handler;
import com.iflytek.aiui.demo.chat.model.handler.HintHandler;
import com.iflytek.aiui.demo.chat.model.handler.JokeHandler;
import com.iflytek.aiui.demo.chat.model.handler.NotificationHandler;
import com.iflytek.aiui.demo.chat.model.handler.OrderMenuHandler;
import com.iflytek.aiui.demo.chat.model.handler.PlayerHandler;
import com.iflytek.aiui.demo.chat.model.handler.RadioDisposer;
import com.iflytek.aiui.demo.chat.model.handler.StoryHandler;
import com.iflytek.aiui.demo.chat.model.handler.TelephoneHandler;
import com.iflytek.aiui.demo.chat.model.handler.WeatherHandler;
import com.iflytek.aiui.demo.chat.ui.PermissionChecker;
import com.iflytek.aiui.demo.chat.ui.chat.ChatViewModel;
import com.iflytek.aiui.demo.chat.ui.chat.PlayerViewModel;
import com.zzhoujay.richtext.callback.OnUrlClickListener;
import com.zzhoujay.richtext.callback.OnUrlLongClickListener;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * Created by PR on 2017/8/4.
 */

public class MessageHandler implements OnUrlClickListener, OnUrlLongClickListener {
    private static final String KEY_SEMANTIC = "semantic";
    private static final String KEY_OPERATION = "operation";
    public static final String SLOTS = "slots";

    private static Map<String, Class> handlerMap = new HashMap<>();

    static {
        handlerMap.put("FOOBAR.DishSkill", DishSkillHandler.class);
        handlerMap.put("FOOBAR.MenuSkill", OrderMenuHandler.class);
        handlerMap.put("musicX", PlayerHandler.class);
        handlerMap.put("cmd", PlayerHandler.class);
        handlerMap.put("story", StoryHandler.class);
        handlerMap.put("joke", JokeHandler.class);
        handlerMap.put("radio", RadioDisposer.class);
        handlerMap.put("telephone", TelephoneHandler.class);
        handlerMap.put("weather", WeatherHandler.class);
        handlerMap.put("dynamic", DynamicEntityHandler.class);
        handlerMap.put("unknown", HintHandler.class);
        handlerMap.put("notification", NotificationHandler.class);
    }


    private ChatViewModel mViewModel;
    private PlayerViewModel mPlayer;
    private PermissionChecker mPermissionChecker;
    private Message mMessage;
    private SemanticResult parsedSemanticResult;
    private Handler mHandler;
    public MessageHandler(ChatViewModel viewModel, PlayerViewModel player, PermissionChecker checker, Message message){
        this.mViewModel = viewModel;
        this.mPlayer = player;
        this.mPermissionChecker = checker;
        this.mMessage = message;
    }

    public String getFormatMessage(){
        initHandler();

        if(mMessage.fromType == Message.FromType.USER){
            if(mMessage.msgType == Message.MsgType.TEXT){
                return new String(mMessage.msgData);
            }else{
                return "";
            }

        }

        if(mHandler != null){
            return mHandler.getFormatContent();
        } else {
            return "错误";
        }
    }

    @Override
    public boolean urlClicked(String url) {
        initHandler();
        return mHandler != null && mHandler.urlClicked(url);
    }

    @Override
    public boolean urlLongClick(String url) {
        initHandler();
        return mHandler != null && mHandler.urlLongClick(url);
    }

    private void initHandler(){
        if(mMessage.fromType == Message.FromType.USER){
            return;
        }

        initSemanticResult();

        if(mHandler == null){
            Class handlerClass = handlerMap.get(parsedSemanticResult.service);
            if(handlerClass == null){
                handlerClass = DefaultHandler.class;
            }
            try {
                Constructor constructor = handlerClass.getConstructor(ChatViewModel.class, PlayerViewModel.class, PermissionChecker.class, SemanticResult.class);
                mHandler = (Handler) constructor.newInstance(mViewModel, mPlayer, mPermissionChecker, parsedSemanticResult);
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (InstantiationException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }
        }
    }

    private void initSemanticResult(){
        if(parsedSemanticResult == null){
            JSONObject semanticResult = null;
            parsedSemanticResult = new SemanticResult();
            try {
                semanticResult = new JSONObject(new String(mMessage.msgData));
                parsedSemanticResult.rc = semanticResult.optInt("rc");
                if(parsedSemanticResult.rc == 4){
                    parsedSemanticResult.service = "unknown";
                }else if(parsedSemanticResult.rc == 1){
                    parsedSemanticResult.service = semanticResult.optString("service");
                    parsedSemanticResult.answer = "语义错误";
                }else{
                    parsedSemanticResult.service = semanticResult.optString("service");
                    parsedSemanticResult.answer = semanticResult.optJSONObject("answer")==null?
                                        "已为您完成操作":semanticResult.optJSONObject("answer").optString("text");
                    boolean isAIUI3_0 = semanticResult.has(KEY_OPERATION);
                    if(isAIUI3_0){
                        JSONObject semantic = semanticResult.optJSONObject(KEY_SEMANTIC);
                        if(semantic != null) {
                            JSONObject slots = semantic.optJSONObject(SLOTS);
                            JSONArray fakeSlots = new JSONArray();
                            Iterator<String> keys = slots.keys();
                            while(keys.hasNext()) {
                                JSONObject item = new JSONObject();
                                String name = keys.next();
                                item.put("name", name);
                                item.put("value", slots.get(name));

                                fakeSlots.put(item);
                            }

                            semantic.put(SLOTS, fakeSlots);
                            semantic.put("intent", semanticResult.optString(KEY_OPERATION));
                            parsedSemanticResult.semantic = semantic;
                        }
                    }else{
                        parsedSemanticResult.semantic = semanticResult.optJSONArray(KEY_SEMANTIC) == null?
                                semanticResult.optJSONObject(KEY_SEMANTIC):
                                semanticResult.optJSONArray(KEY_SEMANTIC).optJSONObject(0);
                    }
                    parsedSemanticResult.answer = parsedSemanticResult.answer.replaceAll("\\[[a-zA-Z0-9]{2}\\]", "");
                    parsedSemanticResult.data = semanticResult.optJSONObject("data");
                }
            } catch (JSONException e) {
                parsedSemanticResult.rc = 4;
                parsedSemanticResult.service = "unknown";
            }


        }
    }
}

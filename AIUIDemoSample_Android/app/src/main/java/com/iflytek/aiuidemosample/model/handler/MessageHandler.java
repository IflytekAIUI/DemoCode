package com.iflytek.aiuidemosample.model.handler;

import com.iflytek.aiuidemosample.model.Message;
import com.iflytek.aiuidemosample.model.data.SemanticResult;
import com.zzhoujay.richtext.callback.OnUrlClickListener;
import com.zzhoujay.richtext.callback.OnUrlLongClickListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by PR on 2017/8/4.
 */

public class MessageHandler implements OnUrlClickListener, OnUrlLongClickListener {
    private static Map<String, Class> handlerMap = new HashMap<>();
    private static String defaultAnswer = "";


    static {
        handlerMap.put("FOOBAR.DishSkill", DishSkillHandler.class);
        handlerMap.put("FOOBAR.MenuSkill", OrderMenuHandler.class);
        handlerMap.put("dynamic", DynamicEntityHandler.class);

        StringBuilder temp = new StringBuilder();
        temp.append("你好，我不懂你的意思");
        temp.append(Handler.NEWLINE);
        temp.append(Handler.NEWLINE);
        temp.append("您可以通过如下交互与我玩耍");
        temp.append(Handler.NEWLINE);
        temp.append(Handler.NEWLINE);
        temp.append("<strong>我要点菜</strong>");
        temp.append(Handler.NEWLINE);
        temp.append("<strong>打电话给XXX</strong>");
        temp.append(Handler.NEWLINE);
        temp.append("<strong>有哪些川菜</strong>");
        defaultAnswer = temp.toString();
    }


    private Message mMessage;
    private SemanticResult parsedSemanticResult;
    private Handler mHandler;
    public MessageHandler(Message message){
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
                Constructor constructor = handlerClass.getConstructor(SemanticResult.class);
                mHandler = (Handler) constructor.newInstance(parsedSemanticResult);
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
                if(parsedSemanticResult.rc != 0){
                    String sid = semanticResult.optString("sid");
                    parsedSemanticResult.answer = sid + "<br/><br/>" + defaultAnswer;
                }else{
                    parsedSemanticResult.service = semanticResult.optString("service");
                    parsedSemanticResult.answer = semanticResult.optJSONObject("answer")==null?
                                        "":semanticResult.optJSONObject("answer").optString("text");
                    parsedSemanticResult.semantic = semanticResult.optJSONArray("semantic") == null?
                            semanticResult.optJSONObject("semantic"):
                            semanticResult.optJSONArray("semantic").optJSONObject(0);
                    parsedSemanticResult.data = semanticResult.optJSONObject("data");
                }
            } catch (JSONException e) {
                parsedSemanticResult.rc = 4;
                parsedSemanticResult.answer = e.getMessage();
            }


        }
    }
}

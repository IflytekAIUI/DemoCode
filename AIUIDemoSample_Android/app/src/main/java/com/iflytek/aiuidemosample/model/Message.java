package com.iflytek.aiuidemosample.model;

import android.arch.persistence.room.Entity;
import android.arch.persistence.room.Ignore;
import android.arch.persistence.room.PrimaryKey;

import com.iflytek.aiuidemosample.model.handler.MessageHandler;

/**
 * Created by PR on 2017/7/26.
 */
@Entity
public class Message {
    public enum MsgType {
        TEXT, Voice
    }

    public enum FromType {
        USER, AIUI
    }

    @PrimaryKey
    public long timestamp;
    public FromType fromType;
    public MsgType msgType;
    public String cacheContent;
    public byte[] msgData;

    @Ignore
    public MessageHandler handler;

    public Message(FromType fromType, MsgType msgType, byte[] msgData, String cacheContent
            , long timestamp){
        this.fromType = fromType;
        this.msgType = msgType;
        this.msgData = msgData;
        this.timestamp = timestamp;
        this.handler = new MessageHandler(this);
        if(cacheContent == null){
            this.cacheContent = handler.getFormatMessage();
        }else{
            this.cacheContent = cacheContent;
        }
    }

    @Ignore
    public Message(FromType fromType, MsgType msgType, byte[] msgData){
        this(fromType, msgType, msgData, null, System.currentTimeMillis());
    }

    public String toTextContent(){
        return cacheContent;
    }

    public boolean isText(){
        return msgType == MsgType.TEXT;
    }

    public boolean isFromUser(){
        return fromType == FromType.USER;
    }

    public String getAudioLen(){
        return Math.round(msgData.length * 1.0f/16000/2) + "s";
    }

}

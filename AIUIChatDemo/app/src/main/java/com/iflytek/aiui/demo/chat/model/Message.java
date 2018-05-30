package com.iflytek.aiui.demo.chat.model;

import android.arch.persistence.room.Entity;
import android.arch.persistence.room.Ignore;
import android.arch.persistence.room.PrimaryKey;
import android.text.TextUtils;

import java.nio.ByteBuffer;

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

    public Message(FromType fromType, MsgType msgType, byte[] msgData, String cacheContent
            , long timestamp) {
        this.fromType = fromType;
        this.msgType = msgType;
        this.msgData = msgData;
        this.timestamp = timestamp;
        this.cacheContent = cacheContent;
    }

    @Ignore
    public Message(FromType fromType, MsgType msgType, byte[] msgData) {
        this(fromType, msgType, msgData, null, System.currentTimeMillis());
    }

    public boolean isText() {
        return msgType == MsgType.TEXT;
    }

    public boolean isEmptyContent() {return TextUtils.isEmpty(cacheContent);}

    public boolean isFromUser() {
        return fromType == FromType.USER;
    }

    public int getAudioLen() {
        if(msgType == MsgType.Voice){
            return Math.round(ByteBuffer.wrap(msgData).getFloat());
        } else {
            return 0;
        }
    }
}

package com.iflytek.aiui.demo.chat.db;

import android.arch.persistence.room.TypeConverter;

import com.iflytek.aiui.demo.chat.model.Message;

/**
 * Created by PR on 2017/8/3.
 */

public class Converters {
    @TypeConverter
    public int msgTypeConvert(Message.MsgType type){
        return type.ordinal();
    }

    @TypeConverter
    public Message.MsgType msgTypeConvert(int type){
        return Message.MsgType.values()[type];
    }

    @TypeConverter
    public int fromTypeConvert(Message.FromType type){
        return type.ordinal();
    }

    @TypeConverter
    public Message.FromType fromTypeConvert(int type){
        return Message.FromType.values()[type];
    }
}

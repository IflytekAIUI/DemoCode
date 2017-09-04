package com.iflytek.aiuidemosample.db;

import android.arch.persistence.room.Database;
import android.arch.persistence.room.RoomDatabase;
import android.arch.persistence.room.TypeConverters;

import com.iflytek.aiuidemosample.model.Message;

/**
 * Created by PR on 2017/8/3.
 */
@Database(entities = {Message.class}, version = 1)
@TypeConverters({Converters.class})
public abstract class MessageDB extends RoomDatabase{
    public abstract MessageDao messageDao();
}

package com.iflytek.aiui.demo.chat.repository;

import android.content.Context;

import java.io.IOException;
import java.io.InputStream;

import javax.inject.Inject;
import javax.inject.Singleton;

/**
 * Created by PR on 2017/12/19.
 */

@Singleton
public class Storage {
    private Context mContext;
    @Inject
    public Storage(Context context) {
       mContext = context;
    }

    public String readAssetFile(String filename){
        try {
            StringBuilder result = new StringBuilder();
            InputStream input = mContext.getAssets().open(filename);
            byte[] buffer = new byte[1024];
            int read;
            while((read = input.read(buffer)) > 0){
                result.append(new String(buffer, 0, read));
            }

            return result.toString();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }
}

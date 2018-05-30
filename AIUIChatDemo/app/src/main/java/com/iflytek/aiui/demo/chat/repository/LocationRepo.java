package com.iflytek.aiui.demo.chat.repository;

import android.arch.lifecycle.LiveData;
import android.content.Context;
import com.iflytek.aiui.demo.chat.ui.common.SingleLiveEvent;
import com.iflytek.location.LocationListener;
import com.iflytek.location.PosLocator;
import com.iflytek.location.result.GPSLocResult;
import com.iflytek.location.result.LocResult;
import com.iflytek.location.result.NetLocResult;

import javax.inject.Inject;
import javax.inject.Singleton;

/**
 * Created by PR on 2018/1/30.
 */

@Singleton
public class LocationRepo {
    private Context mContext;
    private SingleLiveEvent<NetLocResult> mNetLocData = new SingleLiveEvent<>();
    private SingleLiveEvent<GPSLocResult> mGPSLocData = new SingleLiveEvent<>();

    @Inject
    public LocationRepo(Context context) {
        mContext = context;
    }

    public LiveData<NetLocResult> getNetLoc() {
        PosLocator.getInstance(mContext).asyncGetLocation(PosLocator.TYPE_NET_LOCATION, new LocationListener() {
            @Override
            public void onResult(LocResult locResult) {
               mNetLocData.postValue((NetLocResult) locResult);
            }
        });

        return mNetLocData;
    }

    public LiveData<GPSLocResult> getGPSLoc() {
        PosLocator.getInstance(mContext).asyncGetLocation(PosLocator.TYPE_GPS_LOCATION, new LocationListener() {
            @Override
            public void onResult(LocResult locResult) {
                mGPSLocData.postValue((GPSLocResult) locResult);
            }
        });

        return mGPSLocData;
    }
}

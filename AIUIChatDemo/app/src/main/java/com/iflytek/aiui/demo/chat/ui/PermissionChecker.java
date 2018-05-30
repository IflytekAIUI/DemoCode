package com.iflytek.aiui.demo.chat.ui;

/**
 * Created by PR on 2018/1/4.
 */

public interface PermissionChecker {
    public void checkPermission(String permission, Runnable success, Runnable failed);
}

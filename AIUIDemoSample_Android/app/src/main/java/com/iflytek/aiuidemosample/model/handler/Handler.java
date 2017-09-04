package com.iflytek.aiuidemosample.model.handler;

import com.iflytek.aiuidemosample.model.data.SemanticResult;
import com.zzhoujay.richtext.callback.OnUrlClickListener;
import com.zzhoujay.richtext.callback.OnUrlLongClickListener;

/**
 * Created by PR on 2017/8/4.
 */

public abstract class Handler implements OnUrlClickListener, OnUrlLongClickListener {
    public static final String NEWLINE = "<br/>";

    protected SemanticResult result;
    public Handler(SemanticResult result){
        this.result = result;
    }

    public abstract String getFormatContent();

    @Override
    public boolean urlClicked(String url) {
        return false;
    }

    @Override
    public boolean urlLongClick(String url) {
        return false;
    }
}

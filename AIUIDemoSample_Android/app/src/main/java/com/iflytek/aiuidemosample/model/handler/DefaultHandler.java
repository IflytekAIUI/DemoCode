package com.iflytek.aiuidemosample.model.handler;

import com.iflytek.aiuidemosample.model.data.SemanticResult;

/**
 * Created by PR on 2017/8/4.
 */

public class DefaultHandler extends Handler {
    public DefaultHandler(SemanticResult result) {
        super(result);
    }

    @Override
    public String getFormatContent() {
        return result.answer;
    }
}

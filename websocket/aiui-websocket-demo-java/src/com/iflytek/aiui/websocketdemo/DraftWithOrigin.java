package com.iflytek.aiui.websocketdemo;

import org.java_websocket.drafts.Draft;
import org.java_websocket.drafts.Draft_6455;
import org.java_websocket.handshake.ClientHandshakeBuilder;

public class DraftWithOrigin extends Draft_6455 {

    private String originUrl;
    
    public DraftWithOrigin(String originUrl) {
        this.originUrl = originUrl;
    }
    
    @Override
    public Draft copyInstance() {
        System.out.println(originUrl);
        return new DraftWithOrigin(originUrl);
    }

    @Override
    public ClientHandshakeBuilder postProcessHandshakeRequestAsClient( ClientHandshakeBuilder request) {
        super.postProcessHandshakeRequestAsClient(request);
        request.put("Origin", originUrl);
        return request;
    }
}
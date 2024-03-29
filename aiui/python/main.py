#!/bin/env python3
# -*- coding: UTF-8 -*-

import time
import pyaiui
# 为什么是json5, 因为json5可以解析有注释的json
import json5
import os
import hashlib
import uuid

print(pyaiui.aiui_get_version())

AIUI_DIR = './AIUI/'
CFG_PATH = './AIUI/cfg/aiui.cfg'
TEST_AUDIO_PATH = './AIUI/audio/test.pcm'

pyaiui.AIUISetting.setAIUIDir(AIUI_DIR)
pyaiui.AIUISetting.setNetLogLevel(pyaiui.LogLevel.Debug)
pyaiui.AIUISetting.setLogLevel(pyaiui.LogLevel.Debug)

f = open(CFG_PATH, encoding="utf-8")
cfg_str = f.read()
f.close()

cfg = json5.loads(cfg_str)

class MyEventLister(pyaiui.AIUIEventListener):
    def OnEvent(self, event):
        evetType = event.getEventType()
        if evetType == pyaiui.AIUIConstant.EVENT_STATE:
            arg1 = event.getArg1()
            if arg1 == pyaiui.AIUIConstant.STATE_IDLE:
                print("EVENT_STATE: IDLE")
            elif arg1 == pyaiui.AIUIConstant.STATE_READY:
                print("EVENT_STATE: READY")
            elif arg1 == pyaiui.AIUIConstant.STATE_WORKING:
                print("EVENT_STATE: STATE_WORKING")
        elif evetType == pyaiui.AIUIConstant.EVENT_VAD:
            arg1 = event.getArg1()
            if arg1 == pyaiui.AIUIConstant.VAD_BOS:
                print("VAD_BOS")
            elif arg1 == pyaiui.AIUIConstant.VAD_EOS:
                print("VAD_EOS")
        elif evetType == pyaiui.AIUIConstant.EVENT_WAKEUP:
            print("EVENT_WAKEUP: {0}".format(event.getInfo()))
        elif evetType == pyaiui.AIUIConstant.EVENT_SLEEP:
            print("EVENT_SLEEP: arg1={0}".format(event.getArg1()))
        elif evetType == pyaiui.AIUIConstant.EVENT_RESULT:
            info = json5.loads(event.getInfo().encode("utf-8"))
            datas = info["data"]
            data0 = datas[0]
            param = data0["params"]
            contents = data0["content"]
            content0 = contents[0]

            sub = param["sub"]

            dataBundle = event.getData()
            cnt_id = content0["cnt_id"]

            if sub == "nlp":
                resultStr = dataBundle.getBinaryAsStr(cnt_id)

                resultJson = json5.loads(resultStr)

                if 'rc' in resultJson['intent']:
                    print('nlp: {0}'.format(resultJson['intent']['text']))
                    resultStr = dataBundle.getBinaryAsStr(cnt_id)

                    if 'answer' in resultJson['intent']:
                        print('resultStr: {0}'.format(resultJson['intent']['answer']['text']))
                    else:
                        print('resultStr: {0}'.format(resultStr))
                    
            elif sub == "tts":
                if content0["url"] == "1":
                    
                    resultStr = dataBundle.getBinaryAsStr(cnt_id)
                    
                    print('resultStr: {0}'.format(resultStr))
                else:
                    buffer = dataBundle.getBinary(cnt_id)
                    
                    sessionid = dataBundle.getString("sid", "tts")
                    
                    with open(sessionid + ".pcm", 'ab+') as tts:
                        tts.write(buffer)
            else:
                resultStr = dataBundle.getBinaryAsStr(cnt_id)

                print('{0}: {1}\nresultStr: {2}'.format(sub, event.getInfo(), resultStr))

        elif evetType == pyaiui.AIUIConstant.EVENT_ERROR:
            print("Error code: {0}, info: {1}".format(event.getArg1(), event.getInfo()))


def get_auth_id():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    return hashlib.md5(":".join([mac[e:e + 2] for e in range(0, 11, 2)]).encode("utf-8")).hexdigest()


# 为每一个设备设置对应唯一的SN（最好使用设备硬件信息(mac地址，设备序列号等）生成），
# 以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数
# 以下仅测试，不可用于生产环境
pyaiui.AIUISetting.setSystemInfo("sn", get_auth_id())

print(json5.dumps(cfg, quote_keys=True))

# 创建语音交互代理，生命周期要在全局
agent = pyaiui.IAIUIAgent.createAgent(json5.dumps(cfg, quote_keys=True), MyEventLister())


time.sleep(1)
# 一下三行是一个标准的消息发送格式，创建->发送->销毁
start_msg = pyaiui.IAIUIMessage.create(pyaiui.AIUIConstant.CMD_START)
agent.sendMessage(start_msg)
start_msg.destroy()

#使用内部唤醒
time.sleep(1)
wakup_msg = pyaiui.IAIUIMessage.create(pyaiui.AIUIConstant.CMD_WAKEUP)
agent.sendMessage(wakup_msg)
wakup_msg.destroy()

with open(TEST_AUDIO_PATH, 'rb') as audio:
    while True:
        buf = audio.read(1280)

        if not buf:
            break

        data = pyaiui.Buffer.create(buf)
        writeMsg = pyaiui.IAIUIMessage.create(pyaiui.AIUIConstant.CMD_WRITE, 0, 0,
                                              "data_type=audio",
                                              data)
        agent.sendMessage(writeMsg)
        writeMsg.destroy()

        time.sleep(0.04)

stopWriteMsg = pyaiui.IAIUIMessage.create(msgType=pyaiui.AIUIConstant.CMD_STOP_WRITE,
                                      params="data_type=audio")
agent.sendMessage(stopWriteMsg)
stopWriteMsg.destroy()

while True:
    time.sleep(1)

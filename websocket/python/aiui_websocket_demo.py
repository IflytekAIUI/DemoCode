#-*- encoding:utf-8 -*-

import sys
import hashlib
from hashlib import sha1
import hmac
import base64
from socket import *
import json, time, threading
from websocket import create_connection
import websocket
from urllib import quote
import logging

reload(sys)
sys.setdefaultencoding("utf8")
logging.basicConfig()

base_url = "ws://wsapi.xfyun.cn/v1/aiui"
app_id = ""
api_key = ""
# 数据文件位置
file_path = "./date.pcm"
# 数据发送结束标识
end_tag = "--end--"

class Client():
    def __init__(self):
        # 构造握手参数
        curTime = int(time.time())
        param = "{\"auth_id\":\"894c985bf8b1111c6728db79d3479aeg\",\"data_type\":\"audio\",\"aue\":\"raw\",\"scene\":\"main\",\"sample_rate\":\"16000\",\"ver_type\":\"monitor\"}"
        param = param.encode(encoding="utf-8")
        paramBase64 = base64.b64encode(param)
        checkSumPre = api_key + str(curTime) + paramBase64
        checksum = hashlib.md5(checkSumPre.encode("utf-8")).hexdigest()
        connParam = "?appid="+app_id+"&checksum="+checksum+"&param="+paramBase64+"&curtime="+str(curTime)+"&signtype=md5"

        # websocket握手
        self.ws = create_connection(base_url + connParam)
        self.trecv = threading.Thread(target=self.recv)
        self.trecv.start()

    def send(self, file_path):
        file_object = open(file_path, 'rb')
        try:
            while True:
                # 每40ms发送1280个字节数据
                chunk = file_object.read(1280)
                if not chunk:
                    break
                self.ws.send(chunk)

                time.sleep(0.04)
        finally:
            file_object.close()

        # 数据发送结束之后发送结束标识
        self.ws.send(bytes(end_tag))
        print "发送结束标识"

    def recv(self):
        try:
            while self.ws.connected:
                result = str(self.ws.recv())
                if len(result) == 0:
                    print "receive result end"
                    break
                result_dict = json.loads(result)

                print "接收消息："
                print result
        except websocket.WebSocketConnectionClosedException:
            print "接受消息结束"

    def close(self):
        self.ws.close()
        print "连接关闭"

if __name__ == '__main__':
    client = Client()
    client.send(file_path)
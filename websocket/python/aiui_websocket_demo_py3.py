# install ws4py
# pip install ws4py
# easy_install ws4py

from ws4py.client.threadedclient import WebSocketClient
import sys
import hashlib
from hashlib import sha1
import hmac
import base64
import json, time, threading

base_url = "ws://wsapi.xfyun.cn/v1/aiui"
app_id = ""
api_key = ""

file_path = "./date.pcm"
# 结束标识
end_tag = "--end--"

class WsapiClient(WebSocketClient):
    def opened(self):
        file_object = open(file_path, 'rb')
        try:
            while True:
                # 每40ms发送1280个字节数据
                chunk = file_object.read(1280)
                if not chunk:
                    break
                self.send(chunk)

                time.sleep(0.04)
        finally:
            file_object.close()

        # 数据发送结束之后发送结束标识
        self.send(bytes(end_tag.encode("utf-8")))
        print("发送结束标识")

    def closed(self, code, reason=None):
        if code == 1000:
            print("连接正常关闭")
        else:
            print("连接异常关闭，code：" +  str(code) + " ，reason：" + str(reason))

    def received_message(self, m):
        print("接收消息：")
        print(m)

if __name__ == '__main__':
    try:
         # 构造握手参数
        curTime = int(time.time())
        param = "{\"auth_id\":\"894c985bf8b1111c6728db79d3479aeg\",\"data_type\":\"audio\",\"aue\":\"raw\",\"scene\":\"main\",\"sample_rate\":\"16000\",\"ver_type\":\"monitor\"}"
        param = param.encode(encoding="utf-8")
        paramBase64 = base64.b64encode(param).decode()
        checkSumPre = api_key + str(curTime) + paramBase64
        checksum = hashlib.md5(checkSumPre.encode("utf-8")).hexdigest()
        connParam = "?appid="+app_id+"&checksum="+checksum+"&param="+paramBase64+"&curtime="+str(curTime)+"&signtype=md5"

        ws = WsapiClient(base_url + connParam, protocols=['chat'])
        ws.connect()
        ws.run_forever()
    except KeyboardInterrupt:
        ws.close()
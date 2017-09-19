# -*- coding:utf-8 -*-
import base64
import sys
import time
import json
import hashlib
from urllib import request,parse

def webiat():
	print("python version : .{}".format(sys.version))
	requrl = "https://api.xfyun.cn/v1/aiui/v1/iat"
	print('requrl:{}'.format(requrl))
	#讯飞开放平台注册申请应用的应用ID(APPID)
	x_appid = "xxxxxxxx";
	print('X-Appid:{}'.format(x_appid))
	cur_time = int(time.time())
	print('X-CurTime:{}'.format(cur_time))
	x_param = {"auf":"16k","aue":"raw","scene":"main"}
	x_param = json.dumps(x_param)
	xparam_base64 = base64.b64encode(x_param.encode(encoding="utf-8")).decode().strip('\n')
	print('X-Param:{}'.format(xparam_base64))
	#音频文件
	file_data = open('16k.pcm', 'rb')
	file_base64 = base64.b64encode(file_data.read())
	file_data.close()
	body_data = "data="+file_base64.decode("utf-8")
	#ApiKey创建应用时自动生成
	api_key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	token = api_key + str(cur_time)+ xparam_base64 + body_data
	m = hashlib.md5()
	m.update(token.encode(encoding='utf-8'))
	x_check_sum = m.hexdigest()
	print('X-CheckSum:{}'.format(x_check_sum))
	headers = {"X-Appid": x_appid,"X-CurTime": cur_time,"X-Param":xparam_base64,"X-CheckSum":x_check_sum,"Content-Type":"application/x-www-form-urlencoded"}
	print("headers : {}".format(headers))
	req = request.Request(requrl, data=body_data.encode('utf-8'), headers=headers, method="POST")
	with request.urlopen(req) as f:
		body = f.read().decode('utf-8')
		print("result body : {}".format(body))
				
webiat()
# -*- coding:utf-8 -*-
import base64
import sys
import time
import json
import hashlib
import urllib.parse
from urllib import request,parse

UPLOAD_URL = "https://openapi.xfyun.cn/v2/aiui/entity/upload-resource";
CHECK_URL = "https://openapi.xfyun.cn/v2/aiui/entity/check-resource";
X_NONCE = "12";
APPID = "";
X_NAMESPACE = "";
ACCOUNTKEY = "";

def buildHeader():
	curTime = str(int(time.time()))
	m = hashlib.md5()
	m.update((ACCOUNTKEY + X_NONCE + curTime).encode(encoding='utf-8'))
	checkSum = m.hexdigest()

	header = {
		'X-NameSpace': X_NAMESPACE,
		'X-Nonce': X_NONCE,
		'X-CurTime': curTime,
		'X-CheckSum': checkSum,
	}
	return header

def buildUploadBody():
	#每条数据之间用换行符隔开
	param = "{\"name\":\"张三\",\"phoneNumber\":\"13888888888\"}" + "\r\n" + "{\"name\":\"李四\",\"phoneNumber\":\"13666666666\"}"
	paramBase64 = base64.b64encode(param.encode(encoding="utf-8")).decode().strip('\n')
	body = {
		'appid': APPID,
		'res_name': 'IFLYTEK.telephone_contact',
		'pers_param': '{\"auth_id\":\"d3b6d50a9f8194b623b5e2d4e298c9d6\"}',
		'data': paramBase64,
	}
	return body
	
def buildCheckBody(sid):
	body = {
		'sid': sid
	}
	return body

def webEntityDemo():
	body_data = buildUploadBody()
	body_data = urllib.parse.urlencode(body_data).encode('utf-8')
	req = request.Request(UPLOAD_URL, data=body_data, headers=buildHeader(), method="POST")
	f = request.urlopen(req)
	result = f.read().decode('utf-8')
	print("result body : {}".format(result))
	upload_jo = json.loads(result)
	code = upload_jo['code']
	if(code == '00000'):
		#上传资源数据后至少间隔3秒后再进行查看上传资源是否成功
		time.sleep(3)
		sid = upload_jo['data']['sid']
		check_body_data = buildCheckBody(sid)
		check_body_data = urllib.parse.urlencode(check_body_data).encode('utf-8')
		check_req = request.Request(CHECK_URL, data=check_body_data, headers=buildHeader(), method="POST")
		cf = request.urlopen(check_req)
		check_result = cf.read().decode('utf-8')
		print("check result body : {}".format(check_result))

print("python version : .{}".format(sys.version))
webEntityDemo()
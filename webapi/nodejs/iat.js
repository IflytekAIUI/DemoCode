'use strict'; 
var fs = require('fs'); 
var http=require('http');
const crypto = require('crypto');
const hash = crypto.createHash('md5');

var webiat = function(){
	//讯飞开放平台注册申请应用的应用ID(APPID)
	var xAppid = "xxxxxxxx";
	console.log('X-Appid:'+xAppid);
	var timestamp = Date.parse(new Date());  
	var curTime = timestamp / 1000; 
	console.log('X-CurTime:'+curTime);
	var xParam = {"auf": "16k", "aue": "raw", "scene": "main"}
	xParam = JSON.stringify(xParam);
	var xParamBase64 = new Buffer(xParam).toString('base64');
	console.log('X-Param:'+xParamBase64);
	//音频文件
	var fileData = fs.readFileSync('16k.pcm');
	var fileBase64 = new Buffer(fileData).toString('base64');
	var bodyData = "data="+fileBase64;
	//ApiKey创建应用时自动生成
	var apiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	var token = apiKey + curTime+ xParamBase64 + bodyData;
	hash.update(token);
	var xCheckSum = hash.digest('hex');
	console.log('X-CheckSum:'+xCheckSum);
	var options={
		hostname:'api.xfyun.cn',
		port:80,  
		path:'/v1/aiui/v1/iat',
		method:'POST',  
		headers:{
			"X-Appid": xAppid,
			"X-CurTime": curTime,
			"X-Param":xParamBase64,
			"X-CheckSum":xCheckSum,
			'Content-Type':'application/x-www-form-urlencoded; charset=UTF-8'
		}  
	};
	var req = http.request(options, function(res) {
		res.setEncoding('utf-8');
		res.on('data',function(rersult){
			console.info('result body :'+rersult);
		});
		res.on('end',function(){
			console.log('No more data in response.');
		});
	});
	req.on('error',function(err){
		console.error(err);
	});
	req.write(bodyData);
	req.end();
};

webiat();

module.exports = webiat;
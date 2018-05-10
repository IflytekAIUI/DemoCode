const crypto = require('crypto');
const fs = require('fs');  
const path = require('path');  

var http=require('http');  
var querystring=require('querystring');

var APPID = "";
var API_KEY = "";
var AUTH_ID = "2894c985bf8b1111c6728db79d3479ae";
var AUE = "raw";
var CLIENT_IP = "127.0.0.1";
var SAMPLE_RATE = "16000";
var DATA_TYPE = "audio";
var SCENE = "main";
var LAT = "39.938838";
var LNG = "116.368624";
var RESULT_LEVEL = "complete";
var FILE_PATH = ""

var X_CurTime = Math.floor(Date.now()/1000);
var param = "{\"result_level\":\""+RESULT_LEVEL+"\",\"aue\":\""+AUE+"\",\"scene\":\""+SCENE+"\",\"auth_id\":\""+AUTH_ID+"\",\"data_type\":\""+DATA_TYPE+"\",\"sample_rate\":\""+SAMPLE_RATE+"\",\"lat\":\""+LAT+"\",\"lng\":\""+LNG+"\"}";
var X_Param = new Buffer(param).toString('base64'); 
var X_CheckSum = md5(API_KEY+X_CurTime+X_Param);

var options={  
    hostname:'api.xfyun.cn',  
    port:80,  
    path:'/v2/aiui',  
    method:'POST',  
    headers:{
        'X-Param': X_Param,
        'X-CurTime': X_CurTime,
        'X-CheckSum': X_CheckSum,
	    'X-Appid': APPID
   }  
}  

var data = fs.readFileSync(path.resolve(FILE_PATH));  

var req=http.request(options, function(res) {  
    console.log('Status:',res.statusCode);  
    res.setEncoding('utf-8');  
    res.on('data',function(chun){  
        console.log('response：\r\n');  
        console.info(chun);  
    });  
    res.on('end',function(){  
        console.log('end');  
    });  
}); 
 
req.on('error',function(err){  
    console.error(err);  
});  

req.write(data);  
req.end();  

function md5 (text) {
  return crypto.createHash('md5').update(text).digest('hex');
};
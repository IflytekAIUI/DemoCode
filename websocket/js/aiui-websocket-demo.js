var crypto = require('crypto');
var WebSocketClient = require('websocket').client;
var fs = require('fs');

// AIUI websocket服务地址
var BASE_URL = "wss://wsapi.xfyun.cn/v1/aiui";
var ORIGIN = "http://wsapi.xfyun.cn";
// 应用ID，在AIUI开放平台创建并设置
var APPID = "xxxx";
// 接口密钥，在AIUI开放平台查看
var APIKEY = "xxxx";
// 业务参数
var PARAM = "{\"auth_id\":\"f8948af1d2d6547eaf09bc2f20ebfcc6\",\"data_type\":\"audio\",\"scene\":\"main_box\",\"sample_rate\":\"16000\",\"aue\":\"raw\",\"result_level\":\"plain\",\"context\":\"{\\\"sdk_support\\\":[\\\"tts\\\"]}\"}";

// 计算握手参数
function getHandshakeParams(){
    var paramBase64 = new Buffer(PARAM).toString('base64');
    var curtime = Math.floor(Date.now()/1000);
    var originStr = APIKEY + curtime + paramBase64;
    var checksum = crypto.createHash('md5').update(originStr).digest("hex");
    var handshakeParams = "?appid="+APPID+"&checksum="+checksum+"&curtime="+curtime+"&param="+paramBase64;
    console.log(handshakeParams);
    return handshakeParams;
}

// 定义websocket client
var client = new WebSocketClient();
 
client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});
 
client.on('connect', function(connection) {
    console.log('WebSocket client connected');
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('echo-protocol Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log("Received: '" + message.utf8Data + "'");
        }
    });

    function sendMsg() {
        if (connection.connected) {
            // 读取音频文件
            var audioData = fs.readFileSync('D:/output/xxx.pcm', {
              encoding: null,
              flag: 'r'
            })
            // 发送音频数据
            connection.sendBytes(audioData);
            // 发送结束标记
            connection.sendUTF("--end--");
        }
    }
    // 发送数据
    sendMsg();
});

// 建立连接
client.connect(BASE_URL+getHandshakeParams(), "", ORIGIN);
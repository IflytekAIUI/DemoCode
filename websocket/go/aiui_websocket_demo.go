package main

import (
	"fmt"
	"golang.org/x/net/websocket"
	"io/ioutil"
	"strconv"
	"time"
	"net/http"
	"encoding/base64"
	"crypto/md5"
)

// 数据结束发送标记
const BREAK_FLAG = "--end--"
// 每帧数据大小，单位：字节
const SLICE_SIZE = 1280
// AIUI websocket服务地址
const WS_URL = "ws://wsapi.xfyun.cn/v1/aiui"
const ORIGIN = "http://wsapi.xfyun.cn"
// 应用ID，在AIUI开放平台创建并设置
const APPID = ""
// 接口密钥，在AIUI开放平台查看
const API_KEY = ""
// 发送的数据文件位置
const FILE_PATH = "date.pcm"

// 调用方式：运行 main()，控制台输出云端返回结果
func main() {
	curtime :=  strconv.Itoa(int(time.Now().Unix()))
	param := `{"auth_id":"2894c985bf8b1111c6728db79d3479ae","data_type":"audio","aue":"raw","scene":"main","sample_rate":"16000"}`
	paramBase64 := base64.URLEncoding.EncodeToString([]byte(param))
	checksum := Md5Encode(API_KEY + curtime + paramBase64)
	// websocket 握手
	url := WS_URL + "?appid=" + APPID + "&checksum=" + checksum + "&curtime=" + curtime + "&param=" + paramBase64
	config, _ := websocket.NewConfig(url, ORIGIN)
	config.Protocol = []string{"13"}
	header := make(map[string][] string)
	header["X-Real-Ip"] = []string{"114.116.69.134"}
	config.Header = http.Header(header)
	conn, err := websocket.DialConfig(config)
	if err != nil {
		fmt.Errorf("websocket dial err: %v", err)
		return
	}
	defer conn.Close()

	sendChan := make(chan int, 1)
	receiveChan := make(chan int, 1)
	defer close(sendChan)
	defer close(receiveChan)
	// 发送数据
	go send(conn, sendChan)
	// 接收数据
	go receive(conn, receiveChan)
	<-sendChan
	<-receiveChan
	return
}

// 发送数据
func send(conn *websocket.Conn, sendChan chan int) {
	// 分帧发送音频数据
	audio1, _ := ioutil.ReadFile(FILE_PATH)
	if err := sendBySlice(conn, audio1); err != nil {
		fmt.Errorf("send data err: %v", err)
		sendChan <- 1
		return
	}
	// 发送结束符
	if err := websocket.Message.Send(conn, BREAK_FLAG); err != nil {
		fmt.Errorf("send break flag err: %v", err)
		sendChan <- 1
		return
	}

	sendChan <- 1
	return
}

// 分片发送数据
func sendBySlice(conn *websocket.Conn, data []byte) (err error) {
	sliceNum := getSliceNum(len(data), SLICE_SIZE)
	for i := 0; i < sliceNum; i++ {
		var sliceData []byte
		if (i+1)*SLICE_SIZE < len(data) {
			sliceData = data[i*SLICE_SIZE : (i+1)*SLICE_SIZE]
		} else {
			sliceData = data[i*SLICE_SIZE:]
		}
		if err = websocket.Message.Send(conn, sliceData); err != nil {
			fmt.Errorf("send msg err: %v", err)
			return err
		}
		time.Sleep(time.Duration(40 * time.Millisecond))
	}
	return nil
}

// 接收结果
func receive(conn *websocket.Conn, readChan chan int) {
	for {
		var msg string
		if err := websocket.Message.Receive(conn, &msg); err != nil {
			if err.Error() == "EOF" {
				fmt.Println("receive msg end")
			}else{
				fmt.Errorf("receive msg error: %v", msg)
			}
			readChan <- 1
			return
		}
		fmt.Println("receive msg: %v", msg)
	}
	readChan <- 1
}

// 计算数据帧数
func getSliceNum(dataSize, sliceSize int) int {
	if dataSize%sliceSize == 0 {
		return dataSize / sliceSize
	} else {
		return dataSize/sliceSize + 1
	}
}

// 计算字符串MD5值
func Md5Encode(str string) (strMd5 string) {
	strByte := []byte(str)
	strMd5Byte := md5.Sum(strByte)
	strMd5 = fmt.Sprintf("%x", strMd5Byte)
	return strMd5
}
package main

import (
        "bytes"
        "crypto/md5"
        "encoding/base64"
        "fmt"
        "io/ioutil"
        "net"
        "net/http"
        "strconv"
        "time"
)

var URL = "http://openapi.xfyun.cn/v2/aiui"
var APPID = ""
var API_KEY = ""
var SCENE = "main"
var AUE = "raw"
var AUTH_ID = "2894c985bf8b1111c6728db79d3479ae"
var DATA_TYPE = "audio"
var SAMPLE_RATE = "16000"
var LAT = "39.938838";
var LNG = "116.368624";
var RESULT_LEVEL = "complete"
var FILE_PATH = ""

/**
 * AIUI WebAPI V2 接口调用示例
 * 
 * 运行方法：直接运行 main()
 * 
 * 结果： 控制台输出接口返回值信息
 * 
 * @author iflytek_aiui
 * 
 */
func main() {
        dataByte, _ := ioutil.ReadFile(FILE_PATH)
        header := buildHeader()
        result := httpPost(URL,header,dataByte)
        fmt.Printf("接口调用成功，返回值：%v", result)
}

func buildHeader()(header map[string]string)  {
        curTime := strconv.Itoa(int(time.Now().Unix()))
        param := "{\"lat\":\""+LAT+"\",\"lng\":\""+LNG+"\",\"result_level\":\"" + RESULT_LEVEL + "\",\"aue\":\"" + AUE + "\",\"auth_id\":\"" + AUTH_ID + "\",\"data_type\":\"" + DATA_TYPE + "\",\"sample_rate\":\"" + SAMPLE_RATE + "\",\"scene\":\"" + SCENE + "\"}"
        paramBase64 := base64.StdEncoding.EncodeToString([]byte(param))
        checkSum := MD5Encode(API_KEY + curTime + paramBase64)
        header = make(map[string]string)
        header["X-CurTime"] = curTime
        header["X-Param"] = paramBase64
        header["X-CheckSum"] = checkSum
        header["X-Appid"] = APPID
        return header
}

/**
 * 计算字符串MD5值
 */
func MD5Encode(str string) (strMd5 string) {
        strByte := []byte(str)
        strMd5Byte := md5.Sum(strByte)
        strMd5 = fmt.Sprintf("%x", strMd5Byte)
        return strMd5
}

func httpPost(url string,headers map[string]string,body []byte)(result string){
        req, err := http.NewRequest("POST", url, bytes.NewReader(body))
        DefaultClient := http.Client{
                Transport: &http.Transport{
                        Dial: func(netw, addr string) (net.Conn, error) {
                                deadline := time.Now().Add(time.Duration(10000) * time.Millisecond)
                                c, err := net.DialTimeout(netw, addr, time.Duration(TIMEOUT)*time.Millisecond)
                                if err != nil {
                                        return nil, err
                                }
                                c.SetDeadline(deadline)
                                return c, nil
                        },
                },
        }
        //设置请求头
        for k, v := range headers {
                req.Header.Set(k, v)
        }

        resp, err := DefaultClient.Do(req)
        if err != nil {
                fmt.Printf("接口调用失败，错误：%v", err)
                return
        }
        defer resp.Body.Close()
        if resp.StatusCode != http.StatusOK {
                fmt.Printf("接口调用失败，Http 状态码：%v", resp.StatusCode)
                return
        }
        respData, err := ioutil.ReadAll(resp.Body)
        if err != nil {
                fmt.Printf("接口调用失败，错误：%v", resp.StatusCode)
                return
        }
        result = string(respData[:])
        return result
}
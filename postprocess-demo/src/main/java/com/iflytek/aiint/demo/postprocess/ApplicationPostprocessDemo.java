package com.iflytek.aiint.demo.postprocess;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.springframework.http.HttpMethod;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * AIUI应用后处理Java Demo
 * <p>
 * 应用后处理是为开发者提供更方便的后台接入能力，能够根据语音听写或语义等结果，提供个性化的服务和资讯。
 * 应用后处理协议：https://aiui.xfyun.cn/doc/aiui/2_app_setting/postprocessing.html
 *
 * @jdk 1.8
 * @fastjson {@version 1.2.83}
 * @author iflytek
 * @date 2022-06-10
 */
@RestController
@RequestMapping("/application-demo")
public class ApplicationPostprocessDemo {

    /**
     * 开放平台中的token值，替换成开发者自己账号的
     * AIUI平台-应用配置-后处理-校验token
     */
    public final String token = "your token";

    /**
     * 后处理接口
     * 根据请求类型判断接口用途:
     * GET请求是服务器接入验证 {@link #serverAccessVerify}
     * POST请求是接收消息 {@link #receiveMessage}
     *
     * @param httpServletRequest http request
     * @return response body string
     */
    @RequestMapping("/postprocess")
    public String postprocess(HttpServletRequest httpServletRequest) {
        // 获取请求类型,根据请求类型判断接口用途. GET请求是#服务器接入验证; POST请求是#接收消息
        String requestMethod = httpServletRequest.getMethod();
        if (HttpMethod.POST.matches(requestMethod)) {
            // 接收消息
            return receiveMessage(httpServletRequest);
        } else if (HttpMethod.GET.matches(requestMethod)) {
            // 服务器接入验证
            return serverAccessVerify(httpServletRequest);
        } else {
            return null;
        }
    }


    /**
     * 接收消息
     * <p>
     * 1. 消息校验, 非必需。是开发者用来校验消息的完整性。为了消息的安全，建议使用校验
     * 收到消息响应时，可以从url参数中取得以下四个参数，都为字符串类型:
     * msgsignature: 签名信息
     * timestamp: 时间戳
     * rand: 随机字符串
     * encrypttype: 加密类型，由页面配置决定（目前支持raw和aes）。raw表示未加密
     * 校验方式可以使用签名校验方法 {@link #checkSignature}
     * 注意: 当平台勾选加密时，请求给开发者服务器的body是加密后的。需要使用平台提供的AES秘钥进行消息的解密。返回的消息要以同样的方式进行加密。
     * <p>
     * 2. 解析消息主体，根据业务需求做相应处理
     * 消息主体说明:
     * MsgId: 消息id，字符串类型
     * CreateTime: 消息创建时间，整型
     * AppId: 开发者应用Id，字符串类型
     * UserId: AIUI唯一用户标注，字符串类型
     * SessionParams: 本次会话交互参数，Base64格式字符串，解码后为json格式
     * UserParams: 开发者自定义参数，通过客户端的userparams参数上传，Base64格式字符串
     * FromSub: 上游业务类型，目前包括两种（iat：听写结果，kc：语义结果），字符串类型
     * Msg: 消息内容，json object参考Msg消息内容格式
     * <p>
     * Msg消息内容说明:
     * Type: text
     * ContentType: 内容格式，有三种：Json plain xml
     * Content: Base64内容字符串。Content内容需要根据FromSub判断:
     * 1. kc为语义结果，结构参考：https://aiui.xfyun.cn/doc/aiui/4_skill_develop/8_agreement/semantic_agreement.html
     * 2. iat为听写结果
     *
     * @param httpServletRequest http request
     * @return response body string
     */
    public String receiveMessage(HttpServletRequest httpServletRequest) {
        // 消息校验，非必需
        System.out.println("消息验证开始 >>>");

        String msgsignature = httpServletRequest.getParameter("msgsignature");
        String timestamp = httpServletRequest.getParameter("timestamp");
        String rand = httpServletRequest.getParameter("rand");
        String body = readBodyString(httpServletRequest);

        boolean flag = checkSignature(msgsignature, token, rand, timestamp, body);
        if (flag) {
            System.out.println(">>> 消息验证通过");
        } else {
            System.out.println(">>> 消息验证未通过");
            return null;
        }

        // 解析消息主体
        System.out.println("解析消息主体 >>>");
        String response = null;
        if (body != null && body.trim().length() > 0) {
            String contentType = httpServletRequest.getContentType();
            if ("application/json".equals(contentType)) {
                JSONObject jsonBody = JSON.parseObject(body);

                // 解析body,进行业务处理
                String fromSub = jsonBody.getString("FromSub");
                if ("iat".equals(fromSub)) {
                    // 识别结果解析
                    /*
                     * 识别结果示例：
                     *   {
                     *       "SessionParams": "eyJhdWUiOiJyYXciLCJkdHlwZSI6ImF...",
                     *       "Msg": {
                     *           "Type": "text",
                     *           "ContentType": "json",
                     *           "Content": "eyJ0ZXh0Ijp7ImJnIjowLCJlZCI6MCwi..."
                     *       },
                     *       "AppId": "xxxxxx",
                     *       "UserId": "13935267996",
                     *       "CreateTime": 1655104352,
                     *       "UserParams": "",
                     *       "FromSub": "iat", // iat
                     *       "MsgId": "ara8a3119a0@dx000116022f60a10c001"
                     *   }
                     *
                     * Msg.Content Base64解码示例:
                     * {"text":{"bg":0,"ed":0,"ls":false,"sn":1,"ws":[{"bg":0,"cw":[{"sc":0,"w":"合肥"}]},{"bg":0,"cw":[{"sc":0,"w":"明天"}]},{"bg":0,"cw":[{"sc":0,"w":"的"}]},{"bg":0,"cw":[{"sc":0,"w":"天气"}]}]}}
                     */

                    // 若不需要变更识别结果，可以将body原样返回
                    response = body;
                } else if ("kc".equals(fromSub)) {
                    // 语义结果解析

                    /*
                     * 语义结果示例：
                     * {
                     *      "SessionParams": "eyJhdWUiOiJyYXciLCJkdHlw...",
                     *      "Msg": {
                     *          "Type": "text",
                     *          "ContentType": "json",
                     *          "Content": "eyJ0ZXh0Ijp7ImJnIjowLCJlZCI6MCwibHMiOmZhbHN..."
                     *       },
                     *       "AppId": "xxxxxx",
                     *       "UserId": "13935267996",
                     *       "CreateTime": 1655104352,
                     *       "UserParams": "",
                     *       "FromSub": "kc",  // kc
                     *       "MsgId": "ara8a3119a0@dx000116022f60a10c001"
                     * }
                     *
                     * Msg.Content Base64解码示例:
                     * {
                     *       "intent": {
                     *           "answer": {
                     *               "text": "今天合肥市全天阴，气温20℃ ~ 29℃，空气质量优，有东南风，有点热，适合穿短袖短裙等夏季清凉衣物。",
                     *               "type": "T"
                     *           },
                     *           "category": "IFLYTEK.weather",
                     *           "data": {
                     *               "result": [...]  // 16items
                     *           },
                     *           "dialog_stat": "DataValid",
                     *           "rc": 0,
                     *           "save_history": true,
                     *           "semantic": [
                     *               {
                     *                   "intent": "QUERY",
                     *                   "slots": [
                     *                       {
                     *                           "name": "queryType",
                     *                           "value": "内容"
                     *                       },
                     *                       {
                     *                           "name": "subfocus",
                     *                           "value": "天气状态"
                     *                       }
                     *                   ]
                     *               }
                     *           ],
                     *           "service": "weather",
                     *           "sid": "ara24daa655@dx00011602308fa15800",
                     *           "state": {
                     *               "fg::weather::default::default": {
                     *                   "state": "default"
                     *               }
                     *           },
                     *           "text": "合肥今天的天气",
                     *           "used_state": {
                     *               "state": "default",
                     *               "state_key": "fg::weather::default::default"
                     *           },
                     *           "uuid": "ara24daa655@dx00011602308fa15800",
                     *           "version": "162.0"
                     *       }
                     *   }
                     */

                    // 业务处理, 根据需求处理请求，组装response
                    response = handleNlpResult(jsonBody);
                    System.out.println("语义处理结果：" + response);
                }
            }
        }
        System.out.println(">>> 消息解析结束");

        // 如果需要修改后处理结果，返回修改后的body，否则原样返回
        return response == null ?  body : response;
    }


    /**
     * 处理语义结果
     *
     * @param jsonBody request body
     * @return processed body
     */
    public String handleNlpResult(JSONObject jsonBody) {
        // 根据需求组装语义结果
        // 例如: 自定义合成的文本, 需要按照对应格式组装tts参数并返回
        /*
         * 后处理+合成配置时，后处理服务返回结果需要满足下面格式，合成服务才能获取到合成文本
         * {"intent" : { "answer" : {"text":"xxxx"}}}
         */
        String response = null;
        JSONObject msg = jsonBody.getJSONObject("Msg");
        if (msg != null && "json".equals(msg.getString("ContentType"))) {
            String content = new String(Base64.getDecoder().decode(msg.getString("Content")), StandardCharsets.UTF_8);
            // 组成tts参数，这里设置的合成文本为语义结果中answer.text
            JSONObject jsonContent = JSON.parseObject(content);
            JSONObject jsonIntent = jsonContent.getJSONObject("intent");
            Map<String, Object> resMap = new HashMap<>(1);
            Map<String, Object> intent = new HashMap<>(1);
            JSONObject answer = jsonIntent.getJSONObject("answer");
            if (answer != null) {
                intent.put("answer", answer);
            }
            resMap.put("intent", intent);
            response = JSON.toJSONString(resMap);
        }
        // 返回处理后的语义结果
        return response;
    }

    /**
     * 服务器接入验证
     * <p>
     * 开发者接入服务器验证流程：
     * 1. 验证请求，用来确认请求是否来自AIUI服务器 {@link #checkSignature}
     * 2. 验证通过后返回AIUI需要的响应消息: 将token进行sha1加密后的内容字符串放在响应消息的body中返回
     * #校验可选，但消息返回必须正确才能完成服务器接入验证流程
     *
     * @param httpServletRequest 请求
     * @return response string
     */
    public String serverAccessVerify(HttpServletRequest httpServletRequest) {
        System.out.println("服务器接入验证开始 >>>");
        // 获取GET请求携带的参数
        String rand = httpServletRequest.getParameter("rand");
        String timestamp = httpServletRequest.getParameter("timestamp");
        String signature = httpServletRequest.getParameter("signature");

        // 检验signature
        boolean flag = checkSignature(signature, token, rand, timestamp);
        if (flag) {
            System.out.println(">>> 服务器接入验证通过");
            // 验证通过，返回sha1加密的token
            return sha1(token);
        } else {
            System.out.println(">>> 服务器接入验证未通过");
            return null;
        }
    }

    /**
     * 检验signature方法
     * 用于签名验证
     * <p>
     * 加密/校验流程如下：
     * 1. 将参数值args集合进行字典序排序
     * 2. 参数字符串拼接成一个字符串进行sha1加密
     * 3. 开发者获得加密后的字符串可与signature对比，标识该请求来源于AIUI服务
     *
     * @param signature 签名
     * @param args      参数列表
     *                  # token: 开放平台token值 {@link #token}
     *                  # rand: 随机数
     *                  # timestamp: 时间戳
     *                  # data: request body
     * @return boolean of check result
     */
    public boolean checkSignature(String signature, String... args) {
        // 将参数值进行字典序排序
        ArrayList<String> signList = new ArrayList<>();
        Collections.addAll(signList, args);
        Collections.sort(signList);

        // 将参数字符串拼接成一个字符串进行sha1加密
        Iterator<String> it = signList.iterator();
        StringBuilder sb = new StringBuilder();
        while (it.hasNext()) {
            sb.append(it.next());
        }
        String sign = sb.toString();
        String tmpStr = sha1(sign);

        // 加密后的字符串可与signature对比
        return signature != null && signature.equals(tmpStr);
    }

    /**
     * sha1加密方法
     * 可直接使用org.apache.commons.codec.digest.DigestUtils的sha1Hex方法
     *
     * @param str 需要加密的字符串
     * @return encrypted string
     */
    public String sha1(String str) {
        char[] hexDigits = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        MessageDigest mdTemp;
        try {
            mdTemp = MessageDigest.getInstance("SHA-1");
        } catch (NoSuchAlgorithmException e) {
            throw new IllegalArgumentException(e);
        }
        mdTemp.update(str.getBytes(StandardCharsets.UTF_8));
        byte[] md = mdTemp.digest();
        int j = md.length;
        char[] buf = new char[j * 2];
        int k = 0;
        for (byte byte0 : md) {
            buf[k++] = hexDigits[byte0 >>> 4 & 0xf];
            buf[k++] = hexDigits[byte0 & 0xf];
        }
        return new String(buf);
    }

    /**
     * 读取HttpServletRequest的请求体
     *
     * @param httpServletRequest 请求
     * @return String
     */
    public String readBodyString(HttpServletRequest httpServletRequest) {
        BufferedReader reader = null;
        String body = null;
        try {
            httpServletRequest.setCharacterEncoding("utf8");
            reader = httpServletRequest.getReader();
            String line;
            StringBuilder sb = new StringBuilder();
            while ((line = reader.readLine()) != null) {
                sb.append(line);
            }
            body = sb.toString();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return body;
    }
}

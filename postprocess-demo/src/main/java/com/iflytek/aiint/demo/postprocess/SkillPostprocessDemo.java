package com.iflytek.aiint.demo.postprocess;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import org.apache.tomcat.util.codec.binary.Base64;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.BufferedReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;

/**
 * AIUI技能后处理Java Demo
 * <p>
 * 技能后处理流程: 每次客户请求事件会触发技能后处理。
 * 1. 引擎将每次的语义理解结果（request：JSON）发送给技能后处理。
 * 2. 开发者加工完数据，返回一个处理结果（response：JSON）。
 * 3. 引擎通过解析 response 来决定最终下发到客户端的数据。
 * <p>
 * Webhook方式Java Demo,基于Request_v2.1协议
 * 平台协议：https://aiui.xfyun.cn/doc/aiui/4_skill_develop/4_private_skill/postprocess.html
 *
 * @jdk 1.8
 * @fastjson {@version 1.2.83}
 * @author iflytek
 * @date 2022-06-13
 */
@RestController
@RequestMapping("/skill-demo")
public class SkillPostprocessDemo {

    /**
     * 从平台拿到的密钥
     * -----BEGIN PUBLIC KEY----- 与 -----END PUBLIC KEY----- 中间部分字符串
     */
    public final String publicKey = "your key";


    /**
     * 后处理接口
     * <p>
     * 1. 请求校验
     * 2. 解析AIUI发送的语义理解结果，根据业务需求加工数据，返回一个处理结果（response：JSON）
     *
     * @param httpServletRequest  http请求
     * @param httpServletResponse http响应
     * @throws IOException http响应io异常
     */
    @RequestMapping("/postprocess")
    public void postprocess(HttpServletRequest httpServletRequest, HttpServletResponse httpServletResponse) throws IOException {
        String signature = httpServletRequest.getHeader("Signature");
        String body = readBodyString(httpServletRequest);

        // 请求校验
        boolean flag = checkSignature(publicKey, signature, body);
        if (flag) {
            // 通过验证，处理请求
            String response = handleRequest(body);

            // 返回response
            httpServletResponse.setStatus(200);
            httpServletResponse.setContentType("appplication/json");
            httpServletResponse.setCharacterEncoding("UTF-8");
            httpServletResponse.getWriter().print(response);
        } else {
            // 验证未通过
            httpServletResponse.setStatus(400);
            httpServletResponse.getWriter().print("");
        }
    }

    /**
     * 处理请求
     * <p>
     * 自定义技能开发主要工作:
     * 1. 处理AIUI发送的不同类型的请求
     * 2. 在返回完整回复前选择性地发送一些信息给用户，比如告诉用户系统正在处理中
     * 3. 对用户的请求返回合适的回复
     * <p>
     * 请求类型：
     * 1. 标准请求
     * - LaunchRequest: 用户通过入口词“打开{技能名称}”进入你的技能时，你将会收到这个请求。
     * - TextRequest: 在你的技能中，用户的语音请求未经过 AIUI 解析直接请求你的服务器时，你将会收到这个请求。
     * - IntentRequest: 用户的语音请求经过AIUI解析后请求你的服务器时，你将会收到这个请求。
     * - SessionEndedRequest: 用户主动退出技能结束会话时，你将会收到这个请求。
     * <p>
     * 请求结构: 参考 Request_v2.1协议: https://aiui.xfyun.cn/doc/aiui/4_skill_develop/8_agreement/protocal/request_2.1.html
     * 请求正文参数:
     * version: 请求的版本。类型 String, 定值 2.1。必须出现
     * session: 用户的会话信息，仅包含在标准请求中。类型 Object。非必须
     * context: 设备端状态。类型 Object。必须出现
     * request: 经过AIUI解析的用户请求。类型 Object。必须出现
     *
     * @param requestBody request body
     * @return response json string
     */
    public String handleRequest(String requestBody) {
        /*
         * request body 示例:
         *
         * {
         *     "version": "2.1",
         *     "context": {
         *         "AudioPlayer": "",
         *         "System": {
         *             "application": {
         *                 "applicationId": "OS9071964107.yogiho",
         *                 "enable": true
         *             },
         *             "device": {
         *                 "deviceId": "1655862989770cqgjms",
         *                 "location": {},
         *                 "supportedInterfaces": null
         *             },
         *             "user": {
         *                 "accessToken": "",
         *                 "userId": "1655862989770cqgjms"
         *             }
         *         }
         *     },
         *     "session": {
         *         "new": true,
         *         "sessionId": "4dad9d7f-e06b-4286-a8d8-ce23654fbf29"
         *     },
         *     "request": {
         *         "type": "IntentRequest",
         *         "requestId": "atn07f0b626@dx0001160dc3d2a13700",
         *         "timestamp": "2022-06-22T10:00:50.924Z",
         *         "dialogState": "STARTED",
         *         "query": {
         *             "type": "TEXT",
         *             "original": "我的回合"
         *         },
         *         "intent": {
         *             "name": "turn",
         *             "score": 1,
         *             "confirmationStatus": "NONE",
         *             "slots": null
         *         }
         *     }
         * }
         */

        // 解析请求并根据请业务需求构建response
        String response = buildResponse(requestBody);

        // 返回处理后的body
        return response;
    }


    /**
     * 构建后处理Response body
     * 返回结果默认格式见：https://aiui.xfyun.cn/doc/aiui/4_skill_develop/8_agreement/protocal/response_2.1.html
     * <p>
     * Response_2.1 body 正文参数说明:
     * version: 版本，类型 String,取值 2.1，必须出现
     * response: 返回内容,类型 Object，必须出现
     * sessionAttributes: 下一次请求的session{attributes{}}中回传至技能的属性，类型 Object, 非必须。
     * <p>
     * 返回内容对象 response 参数说明:
     * outputSpeech: 操作返回的语音文本，类型 Object，非必须
     *              - type: 输出语音类型，取值 PlainText, 纯文本
     *              - text: 文本内容
     * reprompt: 若该技能回复需要打开录音收听用户的语音回复，当用户在8秒内没有说话时，设备将推送该语音文本，用于再次提示用户输入。
     *           推送后设备再打开录音8s。若用户依旧没有说话，则会话结束。
     *           类型 Object，非必须
     *           - type: 输出语音类型，取值 PlainText, 纯文本
     *           - text: 文本内容
     * card: 用于向设备的关联APP推送消息。支持三种卡片类型：Simple、Standard、List。
     * directives: 一组指令，用于指定使用特定接口进行设备级别的操作。类型 Array。目前支持以下指令：
     *              - AudioPlayer指令
     *              - Playback指令
     *              - Dialog指令
     *              - Display指令
     *              - resolver.Data 指令，用于自定义技能结果
     * expectSpeech: 该返回是否需要设备打开麦克风进行追问。true代表要追问，默认取值为false
     * shouldEndSession: 该返回是否为会话的终点。true表示会话在响应后结束；false表示会话保持活动状态。如果未提供，则默认为true。
     *                   注意: 我们约定：若回复中包含AudioPlayer，且技能没有过多交互，此处取值必须为true
     *
     * 针对不同的请求的响应格式:
     * - LaunchRequest: 标准响应中的所有内容的均可根据业务需求选择包含
     * - TextRequest: 标准响应中的所有内容的均可根据业务需求选择包含
     * - IntentRequest: 标准响应中的所有内容的均可根据业务需求选择包含
     * - SessionEndedRequest: 根据reason分两种情况:
     *                        - 若reason为USER_INITIATED，可以回复。回复内容中只能包括outputSpeech，且ShouldEndSession取值必须为true。
     *                        - 若reason为ERROR，不可回复。
     *
     * @param request 请求体
     * @return response body
     */
    public String buildResponse(String request) {
        /*
         * response 示例，按该示例格式返回的数据能携带自定义数据:
         * {
         *     "version": "2.1",
         *     "sessionAttributes": {
         *         "key": "value"
         *     },
         *     "response": {
         *         "outputSpeech": {
         *             "type": "PlainText",
         *             "text": "Plain text String to speak"
         *         },
         *         "directives": [
         *                        {
         * 				"type": "resolver.Data",
         * 				"data": [                       // 自定义数据列表
         *                    {
         * 						"key1": "自定义数据1",    // 用户可以根据需求自行增添编写 key:value
         * 						"key2": "自定义数据2"
         *                    }
         * 				]
         *            }
         *         ],
         *         "expectSpeech": true,
         *         "shouldEndSession": true
         *     }
         * }
         */
        // 构建responseBody
        JSONObject responseBody = new JSONObject();

        // version, 定值 2.1
        responseBody.put("version", "2.1");

        // sessionAttributes, 非必须。下一次请求所需的数据，根据自己需求设置
        JSONObject sessionAttributes = new JSONObject();
        sessionAttributes.put("key", "value");
        responseBody.put("sessionAttributes", sessionAttributes);

        // response, 根据请求类型及自己的需求设置
        JSONObject response = new JSONObject();

        // 例如: 想要返回的数据能携带自定义数据，可以构建自定义指令。data中的key:value皆可以按需求自定义
        JSONArray directives = JSONArray.parseArray("[{" +
                "\"type\": \"resolver.Data\"," +
                "\"data\": [{" +
                "\"key1\": \"自定义数据1\"," +
                "\"key2\": \"自定义数据2\"" +
                "}]" +
                "}]");
        response.put("directives", directives);

        // 例如: 想要指定操作返回的语音文本
        JSONObject outputSpeech = JSONObject.parseObject("{" +
                "\"type\": \"PlainText\"," +
                "\"text\": \"自定义的语音文本\"" +
                "}");
        response.put("outputSpeech", outputSpeech);

        responseBody.put("response", response);
        return responseBody.toJSONString();
    }

    /**
     * 检验signature方法
     * 用于签名验证
     * <p>
     * 签名验证流程:
     * 1. 对Signature的值进行Base64-decode,得到decoded_signature
     * 2. 使用SHA-1摘要算法（十六进制编码）对请求Body生成hash
     * 3. 使用RSA算法, 使用公钥public_key对decoded_signature,hash进行校验，摘要类型为sha256
     *
     * @param publicKey 签名公钥 {@link #publicKey}
     * @param signature 签名
     * @param body      请求Body
     * @return boolean of check result
     */
    public boolean checkSignature(String publicKey, String signature, String body) {
        // 对Signature的值进行Base64-decode
        byte[] decodedSignature = Base64.decodeBase64(signature);

        // 对请求Body生成hash
        String hash = sha1(body);

        // 使用公钥public_key对decoded_signature,hash进行校验（SHA256withRSA）
        try {
            KeyFactory keyFactory = KeyFactory.getInstance("RSA");
            byte[] encodedKey = Base64.decodeBase64(publicKey);
            PublicKey pubKey = keyFactory.generatePublic(new X509EncodedKeySpec(encodedKey));
            Signature sign = Signature.getInstance("SHA256withRSA");
            sign.initVerify(pubKey);
            sign.update(hash.getBytes(StandardCharsets.UTF_8));
            return sign.verify(decodedSignature);
        } catch (InvalidKeyException | NoSuchAlgorithmException | SignatureException | InvalidKeySpecException e) {
            e.printStackTrace();
            return false;
        }
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

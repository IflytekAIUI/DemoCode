package com.iflytek.aiui.websocketdemo;

import java.io.RandomAccessFile;
import java.io.UnsupportedEncodingException;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.URI;
import java.nio.ByteBuffer;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Date;
import java.util.Enumeration;
import java.util.concurrent.CountDownLatch;

import org.apache.commons.codec.binary.Base64;
import org.apache.commons.codec.binary.Hex;
import org.java_websocket.WebSocket.READYSTATE;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.drafts.Draft;
import org.java_websocket.handshake.ServerHandshake;

/**
 * 运行方法：运行main()，控制台输出结果
 * 
 * @author Administrator
 *
 */
public class AiuiWsDemo {
	// 服务地址
	private static final String BASE_URL = "ws://wsapi.xfyun.cn/v1/aiui";
	private static final String ORIGIN = "http://wsapi.xfyun.cn";

	// 应用ID，见AIUI开放平台
	private static final String APPID = "";
	// APIKEY，见AIUI开放平台
	private static final String APIKEY = "";
	// 每帧音频数据大小，单位字节
	private static final int CHUNCKED_SIZE = 1280;
	// 音频文件位置
	private static final String FILE_PATH = "D://test/date.pcm";
	// 结束数据发送标记（必传）
	private static final String END_FLAG = "--end--";
	// 配置参数
	private static final String param = "{\"result_level\":\"plain\",\"auth_id\":\"" + getLocalMac() +"\",\"data_type\":\"audio\",\"aue\":\"raw\",\"scene\":\"main_box\",\"sample_rate\":\"16000\"}";

	// main()方法，直接运行，控制台输出服务端结果
	public static void main(String[] args) throws Exception {
		URI url = new URI(BASE_URL + getHandShakeParams());
		DraftWithOrigin draft = new DraftWithOrigin(ORIGIN);
		CountDownLatch countDownLatch = new CountDownLatch(1);
		MyWebSocketClient client = new MyWebSocketClient(url, draft, countDownLatch);
		client.connect();
		
		while (!client.getReadyState().equals(READYSTATE.OPEN)) {
			Thread.sleep(50);
		}
		
		// 发送音频
		byte[] bytes = new byte[CHUNCKED_SIZE];
		try (RandomAccessFile raf = new RandomAccessFile(FILE_PATH, "r")) {
			int len = -1;
			while ((len = raf.read(bytes)) != -1) {
				if (len < CHUNCKED_SIZE) {
					bytes = Arrays.copyOfRange(bytes, 0, len);
				}
				send(client, bytes);
				Thread.sleep(40);
			}
			send(client, END_FLAG.getBytes());
			System.out.println("发送结束标识完成");
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("等待连接关闭");
		countDownLatch.await();
	}

	// 拼接握手参数
	private static String getHandShakeParams() throws UnsupportedEncodingException {
		String paramBase64 = new String(Base64.encodeBase64(param.getBytes("UTF-8")));
		String curtime = System.currentTimeMillis() / 1000L + "";
		String signtype = "sha256";
		String originStr = APIKEY + curtime + paramBase64;
		String checksum = getSHA256Str(originStr);
		String handshakeParam = "?appid=" + APPID + "&checksum=" + checksum + "&curtime=" + curtime + "&param=" + paramBase64 + "&signtype=" + signtype;
		return handshakeParam;
	}

	// 发送数据
	private static void send(WebSocketClient client, byte[] bytes) {
		if (client.isClosed()) {
			throw new RuntimeException("client connect closed!");
		}
		client.send(bytes);
	}

	private static class MyWebSocketClient extends WebSocketClient {

		private CountDownLatch countDownLatch;

		public MyWebSocketClient(URI serverUri, Draft protocolDraft, CountDownLatch countDownLatch) {
			super(serverUri, protocolDraft);
			this.countDownLatch = countDownLatch;
		}

		@Override
		public void onOpen(ServerHandshake handshake) {
			System.out.println("打开连接, code:" + handshake.getHttpStatusMessage());
		}

		@Override
		public void onMessage(String msg) {
			System.out.println(msg);
		}

		@Override
		public void onError(Exception e) {
			System.out.println("连接发生错误：" + e.getMessage() + ", " + new Date());
			e.printStackTrace();
		}

		@Override
		public void onClose(int arg0, String arg1, boolean arg2) {
			System.out.println("链接已关闭" + "," + new Date());
			countDownLatch.countDown();
		}

		@Override
		public void onMessage(ByteBuffer bytes) {
			try {
				System.out.println("服务端返回：" + new String(bytes.array(), "utf-8"));
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}
		}
	}

	// 利用Apache的工具类实现SHA-256加密
	private static String getSHA256Str(String str) {
		MessageDigest messageDigest;
		String encdeStr = "";
		try {
			messageDigest = MessageDigest.getInstance("SHA-256");
			byte[] hash = messageDigest.digest(str.getBytes("UTF-8"));
			encdeStr = Hex.encodeHexString(hash);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return encdeStr;
	}
	
    private static String getLocalMac() {
        StringBuilder sb = new StringBuilder();
        Enumeration<NetworkInterface> allNetInterfaces = null;

        try {
            allNetInterfaces = NetworkInterface.getNetworkInterfaces();
            byte[] mac = null;
            while (allNetInterfaces.hasMoreElements()) {
                NetworkInterface netInterface = allNetInterfaces.nextElement();
                if (netInterface.isLoopback() || netInterface.isVirtual() || netInterface.isPointToPoint() || !netInterface.isUp()) {
                    continue;
                } else {
                    mac = netInterface.getHardwareAddress();
                    if (mac != null) {
                        for (int i = 0; i < mac.length; i++) {
                            sb.append(String.format("%02X%s", mac[i], (i < mac.length - 1) ? "-" : "\n"));
                        }
                    }
                }
            }
        } catch (SocketException e) {
            e.printStackTrace();
        }

        return DigestUtils.md5DigestAsHex(sb.toString().getBytes(StandardCharsets.UTF_8));
    }
}
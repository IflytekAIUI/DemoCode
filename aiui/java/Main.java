import com.iflytek.aiui.*;
import com.iflytek.aiui.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Scanner;

public class Main {
    private static String CURRENT_PATH = new File(Thread.currentThread().getContextClassLoader().getResource("").getPath()).getAbsolutePath() + File.separator;
    // 配置文件路径
    private static final String AIUI_CONFIG_FILE = "./AIUI/cfg/aiui.cfg";

    private static AIUIAgent mAgent;

    public static void main(String[] args){
        help();
        Scanner input=new Scanner(System.in);
        while (true) {
            String cmd=input.next();
            if ("c".equals(cmd)) {
                System.out.println("createAgent");
                createAgent();
            } else if ("w".equals(cmd)) {
                System.out.println("wakeup");
                wakeup();
            } else if ("rw".equals(cmd)) {
                System.out.println("wakeup reset");
                reset_wakeup();
            } else if ("s".equals(cmd)) {
                System.out.println("start");
                start();
            } else if ("st".equals(cmd)) {
                System.out.println("stop");
                stop();
            } else if ("d".equals(cmd)) {
                System.out.println("destroy");
                destroy();
            } else if ("e".equals(cmd)) {
                System.out.println("exit");
                break;
            } else if ("wo".equals(cmd)) {
                System.out.println("start record");
                writeAudio();
            } else if ("so".equals(cmd)) {
                System.out.println("stop record");
                stopAudio();
            } else if ("wrt".equals(cmd)) {
                System.out.println("writeText");
                writeText();
            } else if ("wra".equals(cmd)) {
                System.out.println("writeAudio");
                writeAudioFromLocal();
            }else if ("q".equals(cmd)) {
                destroy();
                break;
            } else if ("help".equals(cmd)) {
                help();
                break;
            } else {
                System.out.println("invalid cmd, input again.");
            }
        }
    }

    /**
     * 初始化实例
     */
    private static void createAgent() {
        // 获取系统是32or64位
        String arch = System.getProperty("sun.arch.data.model");
        // 选取不同的库
        if ("32".equals(arch)) {
            System.out.println("load 32 bit lib");
            AIUISetting.setLibPath(CURRENT_PATH + "x86" + File.separator + "aiui.dll");
        } else {
            System.out.println("load 64 bit lib");
            AIUISetting.setLibPath(CURRENT_PATH + "x64" + File.separator + "aiui.dll");
        }
		
        // aiui工作目录，日志信息在CURRENT_PATH + "AIUI/msc/app.log";
        AIUISetting.setAIUIDir(CURRENT_PATH + "AIUI" + File.separator);
        System.out.println("aiui path :" + AIUISetting.getAIUIDir());
        // 打开日志输出，输出目录为：AIUISetting.getAIUIDir() + "msc/aiui.log"
        AIUISetting.setLogLevel(AIUISetting.LogLevel.info);
        AIUISetting.setNetLogLevel(AIUISetting.LogLevel.info);
		
		// 为每一个设备设置对应唯一的SN（最好使用设备硬件信息(mac地址，设备序列号等）生成），
		// 以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数
		AIUISetting.setSystemInfo(AIUIConstant.KEY_SERIAL_NUM, "12345678");
		
        byte[] buffer = readFile(AIUI_CONFIG_FILE);
        String params = new String(buffer, StandardCharsets.UTF_8);
        // 创建agent
        mAgent = AIUIAgent.createAgent(params, new MyListener());
        System.out.println(Version.getVersion());
    }
    private static void start() {
        if (mAgent != null) {
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_START, 0,0,"", null));
        }
    }

    private static void stop() {
        if (mAgent != null) {
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_STOP, 0,0,"", null));
        }
    }

    /**
     * 开始录音
     */
    private static void writeAudio() {
        if (mAgent != null) {
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_START_RECORD, 0,0,"data_type=audio,interact_mode=continuous,disable_rec=0,tts_res_type=url", null));
        }
    }

    /**
     * 停止录音
     */
    private static void stopAudio() {
        if (mAgent != null) {
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_STOP_RECORD, 0,0,"", null));
        }
    }

    /**
     * 文本输入
     */
    private static void writeText() {
        if (mAgent != null) {
            String text = "帮我购买两张单程票";
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0, "data_type=text,tts_res_type=url", text.getBytes(StandardCharsets.UTF_8)));
        }
    }
    // 测试音频
    private static final String TEST_AUDIO_PATH = "./AIUI/audio/test.pcm";
    /**
     * 外部写入音频
     */
    private static void writeAudioFromLocal() {
        try(InputStream inputStream = new FileInputStream(new File(TEST_AUDIO_PATH))) {
            byte[] buff = new byte[1024];
            int length = 0;
            while ((length = inputStream.read(buff)) > 0) {
                mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0, "data_type=audio", Arrays.copyOf(buff, length)));
                // 必须暂停一会儿模拟人停顿，太快的话后端报错
                Thread.sleep(40);
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
        mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_STOP_WRITE, 0, 0, "data_type=audio", null));
    }


    private static void destroy() {
        if (mAgent != null) {
            mAgent.destroy();
        }
    }

    /**
     * 休眠
     */
    private static void reset_wakeup() {
        mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_RESET_WAKEUP, 0,0,"",null));
    }

    /**
     * 唤醒，进入识别状态
     */
    private static void wakeup() {
        if (mAgent != null) {
            mAgent.sendMessage(new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0,0,"", null));
        }
    }
    private static byte[] readFile(String file) {
        try(InputStream inputStream = new FileInputStream(new File(file))) {
            byte[] buffer = new byte[inputStream.available()];
            inputStream.read(buffer);
            inputStream.close();
            return buffer;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static class MyListener implements AIUIListener {
        @Override
        public void onEvent(AIUIEvent event) {
            switch (event.eventType) {
                case AIUIConstant.EVENT_STATE:
                    String state = "";
                    if (event.arg1 == AIUIConstant.STATE_IDLE) {
                        state = "IDLE";
                    } else if (event.arg1 == AIUIConstant.STATE_READY) {
                        state = "READY";
                    } else if (event.arg1 == AIUIConstant.STATE_WORKING) {
                        state = "WORKING";
                    }
                    System.out.println("EVENT_STATE：" + state);
                    break;
                case AIUIConstant.EVENT_WAKEUP:
                    System.out.println("EVENT_WAKEUP:"+ event.info);
                    break;
                case AIUIConstant.EVENT_SLEEP:
                    System.out.println("EVENT_SLEEP:arg1="+event.arg1);
                    break;
                case AIUIConstant.EVENT_VAD:
                    if (event.arg1 == AIUIConstant.VAD_BOS) {
                        System.out.println("EVENT_VAD:BOS");
                    } else if (event.arg1 == AIUIConstant.VAD_EOS) {
                        System.out.println("EVENT_VAD:EOS");
                    } else if (event.arg1 == AIUIConstant.VAD_VOL) {

                    }

                    break;
                case AIUIConstant.EVENT_RESULT:
                    try {
                        JSONObject bizParamJson = new JSONObject(event.info);
                        JSONObject data = bizParamJson.getJSONArray("data").getJSONObject(0);
                        JSONObject params = data.getJSONObject("params");
                        JSONObject content = data.getJSONArray("content").getJSONObject(0);
                        String sub = params.optString("sub");
                        if (content.has("cnt_id")) {
                            String cnt_id = content.getString("cnt_id");
                            byte[] buffer = event.data.getByteArray(cnt_id);
                            if (buffer == null) {
                                return;
                            }
                            System.out.println(sub + " = " + event.info);
                            if ("tts".equals(sub)) {
                                String isUrl = content.getString("url");
                                if ("1".equals(isUrl)) {
                                    System.out.println(new String(buffer, StandardCharsets.UTF_8));
                                } else {

                                }
                            } else {
                                System.out.println(new String(buffer, StandardCharsets.UTF_8));
                            }
                        }
                    } catch (Throwable e) {
                        e.printStackTrace();
                    }
                    break;
                case AIUIConstant.EVENT_CMD_RETURN:
                    if (event.arg1 == AIUIConstant.CMD_BUILD_GRAMMAR) {
                        if (event.arg2 == 0) {
                            System.out.println("build grammar success.");
                        } else {
                            System.out.println("build grammar error, errcode = " + event.arg2);
                            System.out.println("error reasion is " + event.info);
                        }
                    } else if (event.arg1 == AIUIConstant.CMD_UPDATE_LOCAL_LEXICON) {
                        if (event.arg2 == 0) {
                            System.out.println("update lexicon success");
                        } else {
                            System.out.println("update lexicon error, errcode = " + event.arg2);
                            System.out.println("error reasion is " +event.info);
                        }
                    }
                    break;
                case AIUIConstant.EVENT_ERROR: {
                    System.out.println("EVENT_ERROR:" + event.arg1);
                    System.out.println(" ERROR info is " + event.info);
                } break;
            }
        }
    }
    private static void help() {
        String s = "demo示例为输入命令，调用对应的函数，使用AIUI " +
                "SDK完成文本理解，语义理解，文本合成等功能，如：\r\n" +
                "c命令，创建AIUI代理，与AIUI SDK交互都是通过代理发送消息的方式进行, " +
                "所以第一步必须是输入该命令；\r\n" +
                "s命令，初始化内部组件 第二步；\r\n" +
                "w命令，发送外部唤醒命令唤醒AIUI，AIUI只有在唤醒过后才可以交互；第三步\r\n" +
                "rw命令，发送外部休眠命令 AIUI进入休眠状态\r\n" +
                "wo令，sdk内部启动4麦录音 sdk会返回云端识别的听写，语义结果；\r\n" +
                "so命令，停止内部4麦录音\r\n" +
                "wrt命令，字符串文本写入sdk，sdk会返回云端识别的语义结果；\r\n" +
                "wra命令， 音频文件写入sdk，sdk会返回云端识别的语义结果；\r\n" +
                "help命令，显示本demo提供的示例的介绍；\r\n" +
                "输入c命令后，正常情况返回结果为：\r\n" +
                "EVENT_STATE:READY\r\n" +
                "输入w命令后，正常情况返回结果为: \r\n" +
                "EVENT_WAKEUP\r\n" +
                "EVENT_STATE:WORKING\r\n" +
                "听写，语义，合成等结果在onEvent函数，该函数是结果回调，请仔细研究。\r\n" +
                "内部录音流程 c -> s -> w -> wo\r\n" +
                "input cmd:";
        System.out.println(s);
    }
}

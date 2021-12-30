using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using aiui;
using System.Net.NetworkInformation;
using System.Security.Cryptography;

namespace aiui_csharp_demo
{
    class Program
    {
        private static void onEvent(IAIUIEvent ev)
        {
            switch (ev.GetEventType())
            {
                case AIUIConstant.EVENT_STATE:
                    {
                        switch (ev.GetArg1())
                        {
                            case AIUIConstant.STATE_IDLE:
                                {
                                    Console.WriteLine("EVENT_STATE: IDLE");
                                }
                                break;

                            case AIUIConstant.STATE_READY:
                                {
                                    Console.WriteLine("EVENT_STATE: READY");
                                }
                                break;

                            case AIUIConstant.STATE_WORKING:
                                {
                                    Console.WriteLine("EVENT_STATE: WORKING");
                                }
                                break;
                        }
                    }
                    break;
                case AIUIConstant.EVENT_WAKEUP:
                    {
                        Console.WriteLine("EVENT_WAKEUP: {0}", ev.GetInfo());

                    }
                    break;
                case AIUIConstant.EVENT_SLEEP:
                    {
                        Console.WriteLine("EVENT_WAKEUP: arg1={0}", ev.GetArg1());
                    }
                    break;
                case AIUIConstant.EVENT_VAD:
                    {
                        switch (ev.GetArg1())
                        {
                            case AIUIConstant.VAD_BOS:
                                {
                                    Console.WriteLine("EVENT_VAD: BOS");
                                }
                                break;

                            case AIUIConstant.VAD_EOS:
                                {
                                    Console.WriteLine("EVENT_VAD: EOS");
                                }
                                break;
                        }
                    }
                    break;

                case AIUIConstant.EVENT_RESULT:
                    {
                        var info = JsonConvert.DeserializeObject<Dictionary<object, object>>(ev.GetInfo());

                        var datas = info["data"] as JArray;
                        var data = datas[0] as JObject;
                        var param = data["params"] as JObject;
                        var contents = data["content"] as JArray;
                        var content = contents[0] as JObject;

                        string sub = param["sub"].ToString();

                        if (sub == "nlp" || sub == "iat" || sub == "tts" || sub == "asr")
                        {
                            Console.WriteLine("info: {0}", ev.GetInfo());
                            
                            string cnt_id = content["cnt_id"].ToString();
                            int dataLen = 0;
                            byte[] buffer = ev.GetData().GetBinary(cnt_id, ref dataLen);

                            if (sub != "tts")
                            {
                                if (dataLen != 0)
                                {
                                    string resultStr = Encoding.UTF8.GetString(buffer);
                                    Console.WriteLine("resultStr: {0}", resultStr);
                                    resultStr = null;
                                }
                            }

                            buffer = null;
                        }

                        datas = null;
                        data = null;
                        param = null;
                        contents = null;
                        content = null;
                        info = null;
 

                        if(sub == "nlp")
                            GC.Collect();
                    }
                    break;
                case AIUIConstant.EVENT_ERROR:
                    Console.WriteLine("EVENT_ERROR: {0} {1}", ev.GetArg1(), ev.GetInfo());
                    break;

            }
        }

        public static string GetMac()
        {
            NetworkInterface[] interfaces = NetworkInterface.GetAllNetworkInterfaces();

            string mac = "";
            foreach (NetworkInterface ni in interfaces)
            {
                if (ni.NetworkInterfaceType != NetworkInterfaceType.Loopback)
                {
                    mac += ni.GetPhysicalAddress().ToString();
                }
            }

            byte[] result = Encoding.Default.GetBytes(mac);
            result = new MD5CryptoServiceProvider().ComputeHash(result);

            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < result.Length; i++)
            {
                builder.Append(result[i].ToString("x2"));
            }

            return builder.ToString();
        }

        static void Main(string[] args)
        {
            string version = IAIUIAgent.Version();
            Console.WriteLine("aiui version is {0}", version);

            AIUISetting.SetNetLogLevel(AIUISetting.LogLevel._debug);
            AIUISetting.SetLogLevel(AIUISetting.LogLevel._debug);
            AIUISetting.SetAIUIDir("AIUI");

            // 为每一个设备设置对应唯一的SN（最好使用设备硬件信息(mac地址，设备序列号等）生成）
            // 以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数
            AIUISetting.setSystemInfo(AIUIConstant.KEY_SERIAL_NUM, GetMac());

            string cfg = File.ReadAllText(".\\AIUI\\cfg\\aiui.cfg");

            IAIUIAgent agent = IAIUIAgent.Create(cfg, onEvent);


            // start 
            //IAIUIMessage msg_start = IAIUIMessage.Create(AIUIConstant.CMD_START, 0, 0, "", IBuffer.Zero);
            //agent.SendMessage(msg_start);
            //msg_start.Destroy();
            //msg_start = null;

            //Thread.Sleep(400);

            //wakeup
            IAIUIMessage msg_wakeup = IAIUIMessage.Create(AIUIConstant.CMD_WAKEUP, 0, 0, "", IBuffer.Zero);
            agent.SendMessage(msg_wakeup);
            msg_wakeup.Destroy();
            msg_wakeup = null;

            Thread.Sleep(400);

            
            //writeText
            /*
            byte[] text = Encoding.UTF8.GetBytes("合肥明天天气怎么样");

            IBuffer buf = IBuffer.FromData(text, text.Length);
            IAIUIMessage msg_write_text = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=text,text_encoding=utf-8", buf);
            agent.SendMessage(msg_write_text);
            msg_write_text.Destroy();

            Thread.Sleep(40);
            */

            /*
           IAIUIMessage msg_start_r = IAIUIMessage.Create(AIUIConstant.CMD_START_RECORD, 0, 0,
               "data_type=audio,interact_mode=continuous", IBuffer.Zero);
           agent.SendMessage(msg_start_r);
           msg_start_r.Destroy();
           msg_start_r = null;

           Console.WriteLine("start record...");

           Console.ReadKey();

           IAIUIMessage msg_stop_r = IAIUIMessage.Create(AIUIConstant.CMD_STOP_RECORD, 0, 0,
               "data_type=audio,interact_mode=continuous", IBuffer.Zero);
           agent.SendMessage(msg_stop_r);
           msg_stop_r.Destroy();
           msg_stop_r = null;

           Console.WriteLine("stop record...");

           Console.ReadKey();
           */


           FileStream pcm = new FileStream(".\\AIUI\\audio\\test.pcm", FileMode.Open, FileAccess.Read);
           byte[] buffur = new byte[1280];


               int count = 0;
               // 从文件读取并显示行，直到文件的末尾 
               while ((count = pcm.Read(buffur, 0, 1280)) != 0)
               {
                   IBuffer buf_1 = IBuffer.FromData(buffur, 1280);
                   IAIUIMessage msg_write_audio = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=audio", buf_1);
                   agent.SendMessage(msg_write_audio);
                   msg_write_audio.Destroy();

                   msg_write_audio = null;
                   buf_1 = null;

                   pcm.Position += count;

                   Thread.Sleep(40);
               }

               Console.WriteLine("finished!");
               pcm.Position = 0;

               Thread.Sleep(5000);



            agent.Destroy();

            Console.ReadKey();

        }
    }
}

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using aiui;
using NAudio.Wave;
using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Net.NetworkInformation;
using System.Security.Cryptography;

namespace aiui_csharp_demo
{
    class Program
    {
        static WaveInEvent s_WaveIn;

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
                        Console.WriteLine("get result-----------------");
                        var info = JsonConvert.DeserializeObject<Dictionary<object, object>>(ev.GetInfo());

                        var datas = info["data"] as JArray;
                        var data = datas[0] as JObject;
                        var param = data["params"] as JObject;
                        var contents = data["content"] as JArray;
                        var content = contents[0] as JObject;

                        string sub = param["sub"].ToString();

                        if (sub == "nlp" || sub == "iat" || sub == "tts" || sub == "asr")
                        {
                            string cnt_id = content["cnt_id"].ToString();
                            int dataLen = 0;
                            byte[] buffer = ev.GetData().GetBinary(cnt_id, ref dataLen);

                            if (sub == "tts")
                            {
                                Console.WriteLine("info: {0}", ev.GetInfo());
                            }
                            else
                            {
                                if (dataLen != 0)
                                {
                                    string resultStr = Encoding.UTF8.GetString(buffer, 0, dataLen);
                                    Console.WriteLine("resultStr: {0}", resultStr);
                                }
                            }
                        }
                    }
                    break;
                case AIUIConstant.EVENT_ERROR:
                    {
                        Console.WriteLine("ERROR: code {0}, info: {1}", ev.GetArg1(), ev.GetInfo());
                    }
                    break;

            }
        }

        public static string GetMac()
        {
            NetworkInterface[] interfaces = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface ni in interfaces)
            {
                if(ni.OperationalStatus == OperationalStatus.Up)
                {
                    byte[] result = Encoding.Default.GetBytes(ni.GetPhysicalAddress().ToString());    //tbPass为输入密码的文本框
                    byte[] output = new MD5CryptoServiceProvider().ComputeHash(result);

                    StringBuilder builder = new StringBuilder();
                    for (int i = 0; i < output.Length; i++)
                    {
                        builder.Append(output[i].ToString("x2"));
                    }

                    return builder.ToString();
                }
   
            }

            return "1234567890987654321234567890987"; //默认值
        }

        static IAIUIAgent agent = null;
        static void Main(string[] args)
        {
            Console.WriteLine("start.......");
            Console.WriteLine("aiui version is {0}", aiui.IAIUIAgent.Version());

            AIUISetting.SetAIUIDir(".\\AIUI\\");
            AIUISetting.SetLogLevel(AIUISetting.LogLevel._debug);
            AIUISetting.SetNetLogLevel(AIUISetting.LogLevel._debug);
            string mac_addr = GetMac();

            Console.WriteLine("mac_addr: {0}", mac_addr);

            AIUISetting.setSystemInfo("sn", mac_addr); // 设备唯一id

            string cfg = File.ReadAllText(".\\AIUI\\cfg\\aiui.cfg");

            agent = IAIUIAgent.Create(cfg, onEvent);

            // start 
            IAIUIMessage msg_start = IAIUIMessage.Create(AIUIConstant.CMD_START, 0, 0, "", aiui.IBuffer.Zero);
            agent.SendMessage(msg_start);
            msg_start = null;

            Console.WriteLine("start.......");
            Thread.Sleep(40);

            //wakeup
            IAIUIMessage msg_wakeup = IAIUIMessage.Create(AIUIConstant.CMD_WAKEUP, 0, 0, "", aiui.IBuffer.Zero);
            agent.SendMessage(msg_wakeup);
            msg_wakeup = null;
            Console.WriteLine("wakeup.......");

            Thread.Sleep(40);

            //writeText
            byte[] text = Encoding.UTF8.GetBytes("合肥明天天气怎么样？");

            IBuffer buf = IBuffer.FromData(text, text.Length);
            IAIUIMessage msg_write_text = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=text,text_encoding=utf-8", buf);
            agent.SendMessage(msg_write_text);
            msg_write_text = null;

            Thread.Sleep(40);
            startRecord(agent);
            /*
            FileStream pcm = new FileStream(".\\AIUI\\audio\\test.pcm", FileMode.Open, FileAccess.Read);

            byte[] buffur = new byte[1280];
            int count = 0;
            // 从文件读取并显示行，直到文件的末尾 
            while ((count = pcm.Read(buffur, 0, 1280)) != 0)
            {
                IBuffer buf_1 = IBuffer.FromData(buffur, 1280);
                IAIUIMessage msg_write_audio = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=audio,interact_mode=continuous", buf_1);
                agent.SendMessage(msg_write_audio);
                msg_write_text.Destroy();

                pcm.Position += count;

                Thread.Sleep(40);
            }
            */

            Console.ReadKey();

            //agent.Destroy();
        }

        static private void startRecord(IAIUIAgent agent)
        {
            s_WaveIn = new WaveInEvent();
            s_WaveIn.WaveFormat = new WaveFormat(16000, 1);

            s_WaveIn.BufferMilliseconds = 1000;
            s_WaveIn.DataAvailable += s_WaveIn_DataAvailable;
            s_WaveIn.StartRecording();
        }

        static void s_WaveIn_DataAvailable(object sender, WaveInEventArgs e)
        {
            int length = e.Buffer.Length;
            byte[] bufferSrc = new byte[length];
            Array.Copy(e.Buffer, bufferSrc, length);

            byte[] bufTmp = new byte[1280];
            int position = 0;
            int dataLen = 1280;
            while (position <= length)
            {
                if (length - position < 1280)
                {
                    dataLen = length - position;
                }

                Buffer.BlockCopy(bufferSrc, position, bufTmp, 0, dataLen);
                position = position + 1280;
                IBuffer buf_1 = IBuffer.FromData(bufTmp, 1280);
                IAIUIMessage msg_write_audio = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=audio", buf_1);
                agent.SendMessage(msg_write_audio);
                msg_write_audio = null;
            }
        } 
    }
}

﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using static turing_csharp_demo.IAIUIAgent;
using System.Threading;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace turing_csharp_demo
{
    class Program
    {

        [DllImport("aiui", EntryPoint = "aiui_get_version", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static IntPtr aiui_get_version();


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

                            if (sub == "tts")
                            {

                            }
                            else
                            {
                                if (dataLen != 0)
                                {
                                    string resultStr = Encoding.UTF8.GetString(buffer);
                                    Console.WriteLine("resultStr: {0}", resultStr);
                                }
                            }
                        }
                    }
                    break;
            }
        }

        static void Main(string[] args)
        {
            IntPtr temp = aiui_get_version();
            string res = Marshal.PtrToStringAnsi(temp).ToString();
            temp = IntPtr.Zero;

            Console.WriteLine("turing version is {0}", res);


            string cfg = File.ReadAllText("./AIUI/cfg/aiui.cfg");

            IAIUIAgent agent = IAIUIAgent.Create(cfg, onEvent);


            // start 
            IAIUIMessage msg_start = IAIUIMessage.Create(AIUIConstant.CMD_START, 0, 0, "", Buffer.Zero);
            agent.SendMessage(msg_start);
            msg_start.Destroy();

            Thread.Sleep(40);

            //wakeup
            IAIUIMessage msg_wakeup = IAIUIMessage.Create(AIUIConstant.CMD_WAKEUP, 0, 0, "", Buffer.Zero);
            agent.SendMessage(msg_wakeup);
            msg_wakeup.Destroy();

            Thread.Sleep(40);

            //writeText
            byte[] text = Encoding.UTF8.GetBytes("合肥明天天气怎么样");

            Buffer buf = Buffer.FromData(text, text.Length);
            IAIUIMessage msg_write_text = IAIUIMessage.Create(AIUIConstant.CMD_WRITE, 0, 0, "data_type=text", buf);
            agent.SendMessage(msg_write_text);
            msg_write_text.Destroy();

            Thread.Sleep(40);

            IAIUIMessage msg_start_r = IAIUIMessage.Create(AIUIConstant.CMD_START_RECORD, 0, 0,
                "data_type=audio,interact_mode=continuous,disable_rec=0", Buffer.Zero);
            agent.SendMessage(msg_start_r);
            msg_start_r.Destroy();

            Console.WriteLine("start record...");

            Console.ReadKey();

            IAIUIMessage msg_stop_r = IAIUIMessage.Create(AIUIConstant.CMD_STOP_RECORD, 0, 0,
                "data_type=audio,interact_mode=continuous,disable_rec=0", Buffer.Zero);
            agent.SendMessage(msg_stop_r);
            msg_stop_r.Destroy();

            Console.WriteLine("stop record...");

            Console.ReadKey();

            agent.Destroy();
        }
    }
}

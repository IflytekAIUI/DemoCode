using System;
using System.Text;
using System.Threading;
using WebSocketSharp;
using System.Security.Cryptography;
using System.IO;

namespace aiui_ws_csharp_demo
{
    class Program
    {
        private static string BASE_URL = "ws://wsapi.xfyun.cn/v1/aiui";
        private static string ORIGIN = "http://wsapi.xfyun.cn";

        private const string APPID = "****";
        private const string APIKEY = "****";
        private const string AUE = "raw";
        private string AUTH_ID = "ac30105366ea460f9ff08ddac0c4f71e";
        private const string DATA_TYPE = "audio";
        private const string SAMPLE_RATE = "16000";
        private const string SCENE = "main";
        private const string RESULT_LEVEL = "complete";
        private const string LAT = "39.938838";
        private const string LNG = "116.368624";

        // 结束数据发送标记（必传）
        private static string END_FLAG = "--end--";
        // 配置参数
        private static string param = "{\"result_level\":\"plain\",\"auth_id\":\"ac30105366ea460f9ff08ddac0c4f71e\",\"data_type\":\"audio\",\"aue\":\"raw\",\"scene\":\"main\",\"sample_rate\":\"16000\"}";

        private WebSocket server;

        Program()
        {
            string url = BASE_URL + getHandShakeParams();

            System.Console.WriteLine("on_init: {0}", url);


            server = new WebSocket(url);
            server.EnableRedirection = true;
            server.EmitOnPing = true;

            server.Origin = ORIGIN;

            server.OnMessage += on_message;
            server.OnOpen += on_open;
            server.OnError += on_error;
            server.OnClose += on_close;

            server.ConnectAsync();
        }

        public static string currentTimeMillis()
        {
            long currenttimemillis = (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds;

            return String.Format("{0}", currenttimemillis / 1000L);
        }

        public static string EncodeBase64(string source)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(source);
            return Convert.ToBase64String(bytes);
        }

        public static string SHA256EncryptString(string data)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(data);
            byte[] hash = SHA256Managed.Create().ComputeHash(bytes);

            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < hash.Length; i++)
            {
                builder.Append(hash[i].ToString("x2"));
            }
            return builder.ToString();
        }

        // 拼接握手参数
        private static string getHandShakeParams()
        {
            string paramBase64 = EncodeBase64(param);
            string curtime = currentTimeMillis();
            string signtype = "sha256";
            string originStr = APIKEY + curtime + paramBase64;
            string checksum = SHA256EncryptString(originStr);
            string handshakeParam = "?appid=" + APPID + "&checksum=" + checksum + "&curtime=" + curtime + "&param=" + paramBase64 + "&signtype=" + signtype;

            return handshakeParam;
        }

        static void Main(string[] args)
        {
            Program p = new Program();


            while (true)
            {
                Thread.Sleep(1000);
            }
        }

        private void on_close(object sender, CloseEventArgs e)
        {
            System.Console.WriteLine("on_close: {0} -> {1}", e.Code, e.Reason);
        }

        private void on_error(object sender, WebSocketSharp.ErrorEventArgs e)
        {
            System.Console.WriteLine("on_error: {0}", e.Message);
        }

        private void on_message(object sender, MessageEventArgs e)
        {
            if (e.IsBinary)
            {
                System.Console.WriteLine(string.Format("Receive Bytes ({1}): {0}", e.Data, e.RawData.Length));
            }
            else if (e.IsText)
            {
                System.Console.WriteLine(string.Format("Receive: {0}", e.Data));
            }
        }

        private void on_open(object sender, EventArgs e)
        {

            if (server.IsAlive)
            {
                System.Console.WriteLine(string.Format("on_open: yes"));

                FileStream pcm = new FileStream("../../test.pcm", FileMode.Open, FileAccess.Read);

                byte[] buffur = new byte[1280];
                int count = 0, offset = 0;
                // 从文件读取并显示行，直到文件的末尾 
                while ((count = pcm.Read(buffur, 0, 1280)) != 0)
                {
                    server.Send(buffur);
                    pcm.Position += count;

                    Thread.Sleep(40);
                }

                server.Send(Encoding.UTF8.GetBytes(END_FLAG));
            }
            else
            {
                System.Console.WriteLine(string.Format("on_open: error"));
            }
        }
    }
}

using System;
using System.Runtime.InteropServices;

namespace aiui
{
    class IAIUIAgent
    {
        public delegate void AIUIMessageCallback(IAIUIEvent ev);

        private IntPtr mAgent = IntPtr.Zero;

        private AIUIMessageCallback messageCallback = null;
        private AIUIMessageCallback_ onEvent_ = null;

        private void OnEvent(IntPtr ev_, IntPtr data)
        {
            messageCallback?.Invoke(new IAIUIEvent(ev_));
        }

        private IAIUIAgent(string param, AIUIMessageCallback cb)
        {
            messageCallback = cb;
            onEvent_ = new AIUIMessageCallback_(OnEvent);
            mAgent = aiui_agent_create(Marshal.StringToHGlobalAnsi(param), onEvent_, IntPtr.Zero);
        }

        public static IAIUIAgent Create(string param, AIUIMessageCallback cb)
        {
            return new IAIUIAgent(param, cb);
        }

        public void SendMessage(IAIUIMessage msg)
        {
            if (IntPtr.Zero != mAgent)
                aiui_agent_send_message(mAgent, msg.Ptr);
        }

        ~IAIUIAgent()
        {
            if (IntPtr.Zero != mAgent)
            {
                aiui_agent_destroy(mAgent);
                mAgent = IntPtr.Zero;
            }
        }

        public static string Version()
        {
            IntPtr temp = aiui_get_version();
            string res = Marshal.PtrToStringAnsi(temp).ToString();
            temp = IntPtr.Zero;

            return res;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void AIUIMessageCallback_(IntPtr ev, IntPtr data);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static IntPtr aiui_agent_create(IntPtr param, AIUIMessageCallback_ cb, IntPtr data);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_agent_send_message(IntPtr agent, IntPtr msg);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_agent_destroy(IntPtr agent);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static IntPtr aiui_get_version();
    }
}
using System;
using System.Runtime.InteropServices;

namespace aiui_csharp_demo
{
    class IAIUIAgent
    {
        public delegate void AIUIMessageCallback(IAIUIEvent ev);

        private IntPtr mAgent = IntPtr.Zero;

        private static AIUIMessageCallback messageCallback;

        private static void onEvent(IntPtr ev_)
        {
            messageCallback(new IAIUIEvent(ev_));
        }

        private IAIUIAgent(AIUIMessageCallback cb, IntPtr agent)
        {
            messageCallback = cb;
            mAgent = agent;
        }

        public static IAIUIAgent Create(string param, AIUIMessageCallback cb)
        {
            return new IAIUIAgent(cb, aiui_agent_create(Marshal.StringToHGlobalAnsi(param), onEvent));
        }

        public void SendMessage(IAIUIMessage msg)
        {
            if (IntPtr.Zero != mAgent)
                aiui_agent_send_message(mAgent, msg.Ptr);
        }

        public void Destroy()
        {
            if (IntPtr.Zero != mAgent)
            {
                aiui_agent_destroy(mAgent);
                mAgent = IntPtr.Zero;
            }
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void AIUIMessageCallback_(IntPtr ev);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static IntPtr aiui_agent_create(IntPtr param, AIUIMessageCallback_ cb);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_agent_send_message(IntPtr agent, IntPtr msg);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_agent_destroy(IntPtr agent);


    }
}
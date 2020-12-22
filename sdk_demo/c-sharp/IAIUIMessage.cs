using System;
using System.Runtime.InteropServices;

namespace turing_csharp_demo
{
    class IAIUIMessage
    {
        public IntPtr Ptr;

        private IAIUIMessage(IntPtr ptr)
        {
            Ptr = ptr;
        }

        public static IAIUIMessage Create(int msgType, int arg1, int arg2, string param, Buffer data)
        {
            IntPtr tmp = aiui_msg_create(msgType, arg1, arg2, Marshal.StringToHGlobalAnsi(param), data.mData);
            return new IAIUIMessage(tmp);
        }

        public void Destroy()
        {
            aiui_msg_destroy(Ptr);
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_msg_create(int msgType, int arg1, int arg2, IntPtr param, IntPtr data);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern void aiui_msg_destroy(IntPtr msg);
    }
}

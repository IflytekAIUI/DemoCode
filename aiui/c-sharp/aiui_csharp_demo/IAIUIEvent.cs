using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace aiui
{
    class IAIUIEvent
    {
        private IntPtr mEvent;

        public IAIUIEvent(IntPtr ev)
        {
            mEvent = ev;
        }

        public int GetEventType()
        {
            return aiui_event_type(mEvent);
        }

        public int GetArg1()
        {
            return aiui_event_arg1(mEvent);
        }

        public int GetArg2()
        {
            return aiui_event_arg2(mEvent);
        }

        public string GetInfo()
        {
            IntPtr temp = aiui_event_info(mEvent);

            int size = aiui_strlen(temp);

            byte[] str_ptr = new byte[size];
            Marshal.Copy(temp, str_ptr, 0, size);

            return System.Text.Encoding.UTF8.GetString(str_ptr);
        }

        public IDataBundle GetData()
        {
            return new IDataBundle(aiui_event_databundle(mEvent));
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_event_type(IntPtr ev);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_event_arg1(IntPtr ev);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_event_arg2(IntPtr ev);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_event_info(IntPtr ev);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_event_databundle(IntPtr ev);

        [DllImport("kernel32", EntryPoint= "lstrlenA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_strlen(IntPtr str);
    }
}



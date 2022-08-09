using System;
using System.Runtime.InteropServices;
using System.Text;

namespace aiui
{
    class IAIUIEvent
    {
        private IntPtr mEvent;
        private IDataBundle mBundle;

        public IAIUIEvent(IntPtr ev)
        {
            mEvent = ev;
            mBundle = new IDataBundle(aiui_event_databundle(mEvent));
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
            int len = aiui_strlen(temp);

            byte[] managedArray = new byte[len];
            Marshal.Copy(temp, managedArray, 0, len);

            string info = Encoding.UTF8.GetString(managedArray);
           
            temp = IntPtr.Zero;
            managedArray = null;

            return info;
        }

        ~IAIUIEvent()
        {
            mEvent = IntPtr.Zero;
            mBundle = null;
        }

        public IDataBundle GetData()
        {
            return mBundle;
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

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_strlen(IntPtr str);
    }
}

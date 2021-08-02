using System;
using System.Runtime.InteropServices;

namespace aiui
{
    class IBuffer
    {
        public IntPtr mData;

        private IBuffer(IntPtr data)
        {
            mData = data;
        }

        public static readonly IBuffer Zero = new IBuffer(IntPtr.Zero);

        public static IBuffer FromData(byte[] data, int len)
        {
            IntPtr tmp = aiui_create_buffer_from_data(ref data[0], len);
            return new IBuffer(tmp);
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_create_buffer_from_data(ref byte data, int len);
    }
}

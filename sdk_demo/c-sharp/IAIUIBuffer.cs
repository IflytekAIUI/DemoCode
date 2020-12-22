using System;
using System.Runtime.InteropServices;

namespace turing_csharp_demo
{
    class Buffer
    {
        public IntPtr mData;

        private Buffer(IntPtr data)
        {
            mData = data;
        }

        public static readonly Buffer Zero = new Buffer(IntPtr.Zero);

        public static Buffer FromData(byte[] data, int len)
        {
            IntPtr tmp = aiui_create_buffer_from_data(ref data[0], len);
            return new Buffer(tmp);
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_create_buffer_from_data(ref byte data, int len);
    }
}

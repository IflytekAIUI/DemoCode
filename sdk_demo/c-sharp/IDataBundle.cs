using System;
using System.Runtime.InteropServices;

namespace turing_csharp_demo
{
    class IDataBundle
    {
        public IntPtr mDataBundle;

        public IDataBundle(IntPtr db)
        {
            mDataBundle = db;
        }

        public int GetInt(string key, int defVal)
        {
            return aiui_db_int(mDataBundle, Marshal.StringToHGlobalAnsi(key), defVal);
        }

        public string GetString(string key, string defVal)
        {
            IntPtr tmp = aiui_db_string(mDataBundle, Marshal.StringToHGlobalAnsi(key), Marshal.StringToHGlobalAnsi(defVal));

            return Marshal.PtrToStringAnsi(tmp);
        }

        public byte[] GetBinary(string key, ref int len)
        {
            IntPtr tmp = aiui_db_binary(mDataBundle, Marshal.StringToHGlobalAnsi(key), ref len);

            byte[] managedArray = new byte[len];
            Marshal.Copy(tmp, managedArray, 0, len);

            return managedArray;
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_db_int(IntPtr db, IntPtr key, int defVal);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_db_string(IntPtr db, IntPtr key, IntPtr defVal);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_db_binary(IntPtr db, IntPtr key, ref int len);
    }
}

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace aiui
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
            IntPtr temp = aiui_db_string(mDataBundle, Marshal.StringToHGlobalAnsi(key), Marshal.StringToHGlobalAnsi(defVal));

            int len = aiui_strlen(temp);

            byte[] managedArray = new byte[len];
            Marshal.Copy(temp, managedArray, 0, len);

            string info = Encoding.UTF8.GetString(managedArray);

            temp = IntPtr.Zero;

            return info;
        }

        public string GetBinaryStr(string key)
        {
            int len = 0;
            IntPtr tmp = aiui_db_binary(mDataBundle, Marshal.StringToHGlobalAnsi(key), ref len);

            if (len == 0) return null;

            byte[] managedArray = new byte[len - 1];
            Marshal.Copy(tmp, managedArray, 0, len - 1);

            string info = Encoding.UTF8.GetString(managedArray);

            tmp = IntPtr.Zero;
            managedArray = null;

            return info;
        }

        public byte[] GetBinary(string key, ref int len)
        {
            IntPtr tmp = aiui_db_binary(mDataBundle, Marshal.StringToHGlobalAnsi(key), ref len);

            byte[] managedArray = new byte[len];
            Marshal.Copy(tmp, managedArray, 0, len);

            return managedArray;
        }

        ~IDataBundle()
        {
            mDataBundle = IntPtr.Zero;
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_db_int(IntPtr db, IntPtr key, int defVal);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_db_string(IntPtr db, IntPtr key, IntPtr defVal);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr aiui_db_binary(IntPtr db, IntPtr key, ref int len);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_strlen(IntPtr str);
    }
}

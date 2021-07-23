using System;
using System.Runtime.InteropServices;

namespace aiui
{
    class IDataBundle
    {
        public IntPtr mDataBundle = IntPtr.Zero;

        public IDataBundle(IntPtr db)
        {
            mDataBundle = db;
        }

        public int GetInt(string key, int defVal)
        {
            if (IntPtr.Zero == mDataBundle)
            {
                throw new NullReferenceException("IDataBundle hold null pointer!");
            }

            return aiui_db_int(mDataBundle, Marshal.StringToHGlobalAnsi(key), defVal);
        }

        public string GetString(string key, string defVal)
        {
            if (IntPtr.Zero == mDataBundle)
            {
                throw new NullReferenceException("IDataBundle hold null pointer!");
            }

            IntPtr tmp = aiui_db_string(mDataBundle, Marshal.StringToHGlobalAnsi(key), Marshal.StringToHGlobalAnsi(defVal));

            int size = aiui_strlen(tmp);
            byte[] str_ptr = new byte[size];
            Marshal.Copy(tmp, str_ptr, 0, size);

            return System.Text.Encoding.UTF8.GetString(str_ptr);
        }

        public byte[] GetBinary(string key, ref int len)
        {
            if (IntPtr.Zero == mDataBundle)
            {
                throw new NullReferenceException("IDataBundle hold null pointer!");
            }

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

        [DllImport("kernel32", EntryPoint = "lstrlenA", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private static extern int aiui_strlen(IntPtr str);
    }
}

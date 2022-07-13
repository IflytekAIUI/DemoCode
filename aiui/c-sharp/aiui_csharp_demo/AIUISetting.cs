using System;
using System.Runtime.InteropServices;

namespace aiui
{
    class AIUISetting
    {
        public enum LogLevel { _debug, _info, _warn, _error, _none };

        public static bool SetAIUIDir(string dir)
        {
            return aiui_set_aiui_dir(Marshal.StringToHGlobalAnsi(dir));
        }

        public static bool SetMscDir(string dir)
        {
            return aiui_set_msc_dir(Marshal.StringToHGlobalAnsi(dir));
        }

        public static bool SetMscCfg(string cfg)
        {
            return aiui_set_msc_cfg(Marshal.StringToHGlobalAnsi(cfg));
        }

        public static bool InitLogger(string dir)
        {
            return aiui_init_logger(Marshal.StringToHGlobalAnsi(dir));
        }

        public static void SetLogLevel(LogLevel level)
        {
            aiui_set_log_level(level);
        }

        public static void SetNetLogLevel(LogLevel level)
        {
            aiui_set_net_log_level(level);
        }

        public static void setSaveDataLog(bool save, int size)
        {
            aiui_set_save_data_log(save, size);
        }

        public static bool setDataLogDir(string dir)
        {
            return aiui_set_data_log_dir(Marshal.StringToHGlobalAnsi(dir));
        }

        public static void setSystemInfo(string key, string val)
        {
            aiui_set_system_info(Marshal.StringToHGlobalAnsi(key), Marshal.StringToHGlobalAnsi(val));
        }

        public static bool setRawAudioDir(string dir)
        {
            return aiui_set_raw_audio_dir(Marshal.StringToHGlobalAnsi(dir));
        }

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_set_aiui_dir(IntPtr szDir);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static IntPtr aiui_get_aiui_dir();

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_agent_destroy(IntPtr agent);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_set_msc_dir(IntPtr szDir);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_set_msc_cfg(IntPtr szDir);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_init_logger(IntPtr szDir);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_set_log_level(LogLevel level);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_set_net_log_level(LogLevel level);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_set_save_data_log(bool save, int logSizeMB);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_set_data_log_dir(IntPtr szDir);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static void aiui_set_system_info(IntPtr key, IntPtr val);

        [DllImport("aiui", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        private extern static bool aiui_set_raw_audio_dir(IntPtr szDir);
    }
}

1. 将在平台下载的aiui库放在此目录下
2. gcc main.c -o main -L. -Iinclude -laiui -Wl,-rpath,'$ORIGIN' -Werror
3. 修改 AIUI/cfg/aiui.cfg 下的 appid
#include "AIUI_C.h"

#include <fstream>
#include <string>
#ifdef WIN32
#    include <windows.h>
#    define AIUI_SLEEP Sleep
#else
#    include <unistd.h>
#    define AIUI_SLEEP(x) usleep(x * 1000)
#endif


void event(const AIUIEvent ae, void *data)
{
    printf("type: %d, info: %s\n", aiui_event_type(ae), aiui_event_info(ae));
}


string readFileAsString(const string& path)
{
    std::ifstream t(path, std::ios_base::in | std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

int main()
{
    string fileParam = readFileAsString("./AIUI/cfg/aiui.cfg");

    auto agent = aiui_agent_create(fileParam.c_str(), event, NULL);

    if(NULL == agent) {
        printf("agent create error");
        return -1;
    }

    AIUI_SLEEP(3);

    auto wakeupMsg = aiui_msg_create(7);
    aiui_agent_send_message(agent, wakeupMsg);
    aiui_msg_destroy(wakeupMsg);


    string text = "合肥明天天气怎么样";

    AIUI_SLEEP(3);

    // textData内存会在Message在内部处理完后自动release掉
    auto textData = aiui_create_buffer_from_data(text.c_str(), text.length());

    auto writeMsg = aiui_msg_create(2, 0, 0, "data_type=text,tts_res_type=url", textData);
    aiui_agent_send_message(agent, writeMsg);
    aiui_msg_destroy(writeMsg);

    while(1){};
}
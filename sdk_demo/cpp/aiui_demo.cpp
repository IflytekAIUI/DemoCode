#ifdef WIN32
#    include <windows.h>
#    define AIUI_SLEEP Sleep
#    define MSPAPI     __stdcall
#else
#    include "dlfcn.h"
#    include <unistd.h>

#    define MSPAPI
#    define AIUI_SLEEP(x) usleep(x * 1000)
#endif

#undef AIUI_LIB_COMPILING

#include "aiui/AIUI.h"
#include "json/json.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <aiui/AIUI_C.h>

typedef int(MSPAPI* Proc_MSPLogin)(const char* usr, const char* pwd, const char* params);
typedef int(MSPAPI* Proc_MSPLogout)();

using namespace std;
using namespace aiui_va;
using namespace aiui;

enum { Text = 0, Audio = 1, Video = 2, Picture = 3 };

class MyListener : public IAIUIListener
{
public:
    void onEvent(const IAIUIEvent& event) const override
    {
        switch (event.getEventType()) {
            //SDK 状态回调
            case AIUIConstant::EVENT_STATE: {
                switch (event.getArg1()) {
                    case AIUIConstant::STATE_IDLE: {
                        cout << "EVENT_STATE:"
                             << "IDLE" << endl;
                    } break;

                    case AIUIConstant::STATE_READY: {
                        cout << "EVENT_STATE:"
                             << "READY" << endl;
                    } break;

                    case AIUIConstant::STATE_WORKING: {
                        cout << "EVENT_STATE:"
                             << "WORKING" << endl;
                    } break;
                }
            } break;

                //唤醒事件回调
            case AIUIConstant::EVENT_WAKEUP: {
                cout << "EVENT_WAKEUP:" << event.getInfo() << endl;
            } break;

                //休眠事件回调
            case AIUIConstant::EVENT_SLEEP: {
                cout << "EVENT_SLEEP:arg1=" << event.getArg1() << endl;
            } break;

                //VAD事件回调，如找到前后端点
            case AIUIConstant::EVENT_VAD: {
                switch (event.getArg1()) {
                    case AIUIConstant::VAD_BOS: {
                        cout << "EVENT_VAD:"
                             << "BOS" << endl;
                    } break;

                    case AIUIConstant::VAD_EOS: {
                        cout << "EVENT_VAD:"
                             << "EOS" << endl;
                    } break;

                    case AIUIConstant::VAD_VOL: {
                        //						cout << "EVENT_VAD:" << "VOL" << endl;
                    } break;
                }
            } break;

                //最重要的结果事件回调
            case AIUIConstant::EVENT_RESULT: {
                Json::Value bizParamJson;
                Json::Reader reader;

                if (!reader.parse(event.getInfo(), bizParamJson, false)) {
                    cout << "parse error!" << endl << event.getInfo() << endl;
                    break;
                }
                Json::Value& data = (bizParamJson["data"])[0];
                Json::Value& params = data["params"];
                Json::Value& content = (data["content"])[0];

                string sub = params["sub"].asString();

                if (sub == "nlp" || sub == "iat" || sub == "tts" || sub == "asr") {
                    Json::Value empty;
                    Json::Value contentId = content.get("cnt_id", empty);

                    if (contentId.empty()) {
                        cout << "Content Id is empty" << endl;
                        break;
                    }

                    string cnt_id = contentId.asString();
                    int dataLen = 0;
                    const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

                    if (sub == "tts") {
                        cout << sub << ": " << event.getInfo() << endl;

                        Json::Value&& isUrl = content.get("url", empty);

                        if (isUrl.asString() == "1") {
                            std::cout << (buffer, dataLen) << std::endl;
                        }
                    } else {
                        if (buffer) {
                            string resultStr = string(buffer, dataLen);
                            cout << sub << ": " << resultStr << endl;
                        }
                    }
                }
            } break;

                //上传资源数据的返回结果
            case AIUIConstant::EVENT_CMD_RETURN: {
                if (AIUIConstant::CMD_BUILD_GRAMMAR == event.getArg1()) {
                    if (event.getArg2() == 0) {
                        cout << "build grammar success." << endl;
                    } else {
                        cout << "build grammar error, errcode = " << event.getArg2() << endl;
                        cout << "error reasion is " << event.getInfo() << endl;
                    }
                } else if (AIUIConstant::CMD_UPDATE_LOCAL_LEXICON == event.getArg1()) {
                    if (event.getArg2() == 0) {
                        cout << "update lexicon success" << endl;
                    } else {
                        cout << "update lexicon error, errcode = " << event.getArg2() << endl;
                        cout << "error reasion is " << event.getInfo() << endl;
                    }
                }
            } break;

            case AIUIConstant::EVENT_ERROR: {
                cout << "EVENT_ERROR:" << event.getArg1() << endl;
                cout << " ERROR info is " << event.getInfo() << endl;
            } break;
        }
    }
};

MyListener listener;

#define TEST_ROOT_DIR   "./AIUI/"
#define CFG_FILE_PATH   "./AIUI/cfg/aiui.cfg"
#define TEST_AUDIO_PATH "./AIUI/audio/weight.pcm"
#define LOG_DIR         "./AIUI/log/"
#define MSC_DIR         "./AIUI/msc/"

string readFileAsString(const string& path)
{
    std::ifstream t(path, std::ios_base::in | std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

streamsize Read(istream& stream, char* buffer, streamsize count)
{
    // This consistently fails on gcc (linux) 4.8.1 with failbit set on read
    // failure. This apparently never fails on VS2010 and VS2013 (Windows 7)
    streamsize reads = stream.rdbuf()->sgetn(buffer, count);

    // This rarely sets failbit on VS2010 and VS2013 (Windows 7) on read
    // failure of the previous sgetn()
    stream.rdstate();

    // On gcc (linux) 4.8.1 and VS2010/VS2013 (Windows 7) this consistently
    // sets eofbit when stream is EOF for the conseguences  of sgetn(). It
    // should also throw if exceptions are set, or return on the contrary,
    // and previous rdstate() restored a failbit on Windows. On Windows most
    // of the times it sets eofbit even on real read failure
    stream.peek();

    return reads;
}

IAIUIAgent* agent = nullptr;

void msc_login()
{
#ifndef AIUI_WINDOWS
    const char* mscLibPath = "libmsc.so";
    typedef void* LOAD_LIBRARY_HANDLE;

#    define LOAD_LIB_FUNC(name)           dlopen(name, RTLD_LAZY)
#    define LOAD_LIB_ERROR()              dlerror()
#    define GET_LIB_ADDRESS(handle, name) dlsym(handle, name)
#    define FREE_LIB_FUNC(handle)         dlclose(handle)
#else
    const char* mscLibPath = "msc.dll";
    typedef HMODULE LOAD_LIBRARY_HANDLE;

#    define LOAD_LIB_FUNC(name)           LoadLibraryA(name)
#    define LOAD_LIB_ERROR()              GetLastError()
#    define GET_LIB_ADDRESS(handle, name) GetProcAddress(handle, name)
#    define FREE_LIB_FUNC(handle)         FreeLibrary(handle)
#endif

    static LOAD_LIBRARY_HANDLE handle = NULL;

    if (!handle) {
        handle = LOAD_LIB_FUNC(mscLibPath);
    }

    if (handle) {
        Proc_MSPLogout mspLogout = (Proc_MSPLogout)GET_LIB_ADDRESS(handle, "MSPLogout");
        Proc_MSPLogin mspLogin = (Proc_MSPLogin)GET_LIB_ADDRESS(handle, "MSPLogin");

        mspLogout();

        mspLogin(NULL, NULL, "appid=******, work_dir=" TEST_ROOT_DIR);
    }
}

void createAgent()
{
    aiui_va::Json::Value paramJson;

    std::cout << CFG_FILE_PATH << std::endl;

    string fileParam = readFileAsString(CFG_FILE_PATH);

    aiui_va::Json::Reader reader;
    if (reader.parse(fileParam, paramJson, false)) {
#ifdef CAE_AUDIO_CAPTOR_SUPPORT
        const char* mic_type = paramJson["speech"]["audio_captor"].asCString();

        if (int err = aiui_init_cae_engine(mic_type)) {
            std::cout << "aiui_init_cae_engine: " << err << std::endl;
        }
#endif
        msc_login();

        agent = IAIUIAgent::createAgent(paramJson.toString().c_str(), &listener);
    }

    if (!agent) {
        std::cout << "create agent fail" << std::endl;
    } else {
        std::cout << "create agent sucess" << std::endl;
    }
}

void wakeup()
{
    if (NULL != agent) {
        IAIUIMessage* wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_WAKEUP);
        agent->sendMessage(wakeupMsg);
        wakeupMsg->destroy();
    }
}

void reset_wakeup()
{
    if (NULL != agent) {
        IAIUIMessage* wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_RESET_WAKEUP);
        agent->sendMessage(wakeupMsg);
        wakeupMsg->destroy();
    }
}

//停止AIUI服务，此接口是与stop()对应，调用stop()之后必须调用此接口才能继续与SDK交互，如果你没有调用过stop(),就不需要调用该接口
void start()
{
    if (NULL != agent) {
        IAIUIMessage* startMsg = IAIUIMessage::create(AIUIConstant::CMD_START);
        agent->sendMessage(startMsg);
        startMsg->destroy();
    }
}

//停止AIUI服务
void stop()
{
    if (NULL != agent) {
        IAIUIMessage* stopMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP);
        agent->sendMessage(stopMsg);
        stopMsg->destroy();
    }
}

// 写入测试音频
void writeAudio()
{
    if (NULL != agent) {
        IAIUIMessage* stopMsg =
            IAIUIMessage::create(AIUIConstant::CMD_START_RECORD,
                                 0,
                                 0,
                                 "data_type=audio,interact_mode=continuous,disable_rec=0,tts_res_type=url");
        agent->sendMessage(stopMsg);
        stopMsg->destroy();
    }
}

void writeAudioFromLocal()
{
    if (NULL != agent) {
        ifstream testData(TEST_AUDIO_PATH, std::ios::in | std::ios::binary);

        if (testData.is_open()) {
            char buff[1280];
            while (testData.good() && !testData.eof()) {
                memset(buff, '\0', 1280);
                testData.read(buff, 1280);

                // textData内存会在Message在内部处理完后自动release掉
                Buffer* textData = Buffer::alloc(1280);
                memcpy((char*)textData->data(), buff, 1280);

                IAIUIMessage* writeMsg = IAIUIMessage::create(
                    AIUIConstant::CMD_WRITE,
                    0,
                    0,
                    "data_type=audio,interact_mode=continuous,tts_res_type=url",
                    textData);

                agent->sendMessage(writeMsg);
                writeMsg->destroy();

                // 必须暂停一会儿模拟人停顿，太快的话后端报错
                AIUI_SLEEP(40);
            }

            testData.close();

            IAIUIMessage* msg = IAIUIMessage::create(
                AIUIConstant::CMD_STOP_WRITE,
                0,
                0,
                "data_type=audio,interact_mode=continuous,vad=1,tts_res_type=url");
            agent->sendMessage(msg);
            msg->destroy();
        }
    }
}

void stopAudio()
{
    if (NULL != agent) {
        IAIUIMessage* stopMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP_RECORD);
        agent->sendMessage(stopMsg);
        stopMsg->destroy();
    }
}

//文本语义测试接口
void writeText()
{
    if (NULL != agent) {
        string text = "合肥明天天气怎么样";

        // textData内存会在Message在内部处理完后自动release掉
        Buffer* textData = Buffer::alloc(text.length());
        text.copy((char*)textData->data(), text.length());

        IAIUIMessage* writeMsg = IAIUIMessage::create(
            AIUIConstant::CMD_WRITE, 0, 0, "data_type=text,tts_res_type=url", textData);

        agent->sendMessage(writeMsg);
        writeMsg->destroy();
    }
}

void testTTs()
{
    if (NULL != agent) {
        string text = "敕勒川，阴山下。天似穹庐，笼盖四野。天苍苍，野茫茫。风吹草低见牛羊。";
        // textData内存会在Message在内部处理完后自动release掉
        Buffer* textData = Buffer::alloc(text.length());
        text.copy((char*)textData->data(), text.length());

        IAIUIMessage* ttsMsg =
            IAIUIMessage::create(AIUIConstant::CMD_TTS, 1, 0, "text_encoding=utf-8", textData);

        agent->sendMessage(ttsMsg);
        ttsMsg->destroy();
    }
}

void testTTs2()
{
    if (NULL != agent) {
        string text = "敕勒川，阴山下。天似穹庐，笼盖四野。天苍苍，野茫茫。风吹草低见牛羊。";
        // textData内存会在Message在内部处理完后自动release掉
        Buffer* textData = Buffer::alloc(text.length());
        text.copy((char*)textData->data(), text.length());

        IAIUIMessage* ttsMsg = IAIUIMessage::create(
            AIUIConstant::CMD_TTS, 1, 0, "text_encoding=utf-8,tts_res_type=url", textData);

        agent->sendMessage(ttsMsg);
        ttsMsg->destroy();
    }
}

void destroy()
{
    if (NULL != agent) {
        agent->destroy();
        agent = NULL;
    }
}

void buildGrammar()
{
    if (NULL != agent) {
        string grammer = readFileAsString("AIUI/asr/call.bnf");

        auto msg =
            IAIUIMessage::create(AIUIConstant::CMD_BUILD_GRAMMAR, 0, 0, grammer.c_str(), NULL);
        agent->sendMessage(msg);
        msg->destroy();
    }
}

void help()
{
    static string s =
        "demo示例为输入命令，调用对应的函数，使用AIUI "
        "SDK完成文本理解，语义理解，文本合成等功能，如：\r\n"
        "c命令，创建AIUI代理，与AIUI SDK交互都是通过代理发送消息的方式进行, "
        "所以第一步必须是输入该命令；\r\n"
        "s命令，初始化内部组件 第二步；\r\n"
        "w命令，发送外部唤醒命令唤醒AIUI，AIUI只有在唤醒过后才可以交互；第三步\r\n"
        "rw命令，发送外部休眠命令 AIUI进入休眠状态\r\n"
        "wo令，sdk内部启动4麦录音 sdk会返回云端识别的听写，语义结果；\r\n"
        "so命令，停止内部4麦录音\r\n"
        "wrt命令，字符串文本写入sdk，sdk会返回云端识别的语义结果；\r\n"
        "wra命令， 音频文件写入sdk，sdk会返回云端识别的语义结果；\r\n"
        "tts命令，单合成示例，返回合成的音频，demo将音频保存为本地的pcm文件；\r\n"
        "stts命令，单合成示例，返回合成的音频的url保存为本地的txt文件；\r\n"
        "bg命令，构建语法\r\n"
        "help命令，显示本demo提供的示例的介绍；\r\n"
        "输入c命令后，正常情况返回结果为：\r\n"
        "EVENT_STATE:READY\r\n"
        "输入w命令后，正常情况返回结果为: \r\n"
        "EVENT_WAKEUP\r\n"
        "EVENT_STATE:WORKING\r\n"
        "听写，语义，合成等结果在onEvent函数，该函数是结果回调，请仔细研究。\r\n"
        "内部录音流程 c -> s -> w -> wo\r\n"
        "input cmd:";

    cout << s << endl;
}

int main()
{
#ifdef WIN32
    system("chcp 65001 >nul");
#endif

    std::cout << "Version: " << aiui::getVersion() << std::endl;

    help();

    AIUISetting::setAIUIDir(TEST_ROOT_DIR);
    AIUISetting::setDataLogDir(LOG_DIR);
    AIUISetting::setMscDir(MSC_DIR);
    AIUISetting::setNetLogLevel(aiui::_error);
    AIUISetting::setLogLevel(aiui::_info);

    string cmd;
    while (true) {
        cin >> cmd;

        if (cmd == "c") {
            cout << "createAgent" << endl;
            createAgent();
        } else if (cmd == "w") {
            cout << "wakeup" << endl;
            wakeup();
        } else if (cmd == "rw") {
            cout << "wakeup" << endl;
            reset_wakeup();
        } else if (cmd == "s") {
            cout << "start" << endl;
            start();
        } else if (cmd == "st") {
            cout << "stop" << endl;
            stop();
        } else if (cmd == "d") {
            cout << "destroy" << endl;
            destroy();
        } else if (cmd == "e") {
            cout << "exit" << endl;
            break;
        } else if (cmd == "wo") {
            cout << "start record" << endl;
            writeAudio();
        } else if (cmd == "so") {
            cout << "stop record" << endl;
            stopAudio();
        } else if (cmd == "wrt") {
            cout << "writeText" << endl;
            writeText();
        } else if (cmd == "wra") {
            cout << "writeAudio" << endl;
            writeAudioFromLocal();
        } else if (cmd == "stts") {
            testTTs2();
        } else if (cmd == "tts") {
            testTTs();
        } else if (cmd == "bg") {
            buildGrammar();
        } else if (cmd == "q") {
            destroy();
            break;
        } else if (cmd == "h") {
            help();
        } else {
            cout << "invalid cmd, input again." << endl;
        }
    }
}

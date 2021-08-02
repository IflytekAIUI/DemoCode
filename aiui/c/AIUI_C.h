/**
 * @Author zrmei
 * @Create 2020/9/6 11:47
 */

#ifndef AIUI_SRC_AIUI_C_H
#define AIUI_SRC_AIUI_C_H

#ifdef _MSC_VER
#   ifndef AIUI_WINDOWS
#       define AIUI_WINDOWS
#   endif
#endif

#if defined(AIUI_WINDOWS)
#	undef AIUIEXPORT
#	if defined(AIUI_LIB_COMPILING)
#		define AIUIEXPORT __declspec(dllexport)
#   	define AIUIAPI    __stdcall
#	else
#   	define AIUIEXPORT 
#   	define AIUIAPI    __stdcall
#	endif
#else
#	undef AIUIEXPORT
#	define AIUIEXPORT __attribute__((visibility ("default")))
#  	undef AIUIAPI
#  	define AIUIAPI
#endif

/* clang-format off */
#ifdef __cplusplus
extern "C" {
#endif

const char AIUIEXPORT * AIUIAPI aiui_get_version();

/*******************************AIUIDataBundle********************************/
typedef void* AIUIDataBundle;

int AIUIEXPORT AIUIAPI aiui_db_int(AIUIDataBundle db, const char* key, int defaultVal);
long AIUIEXPORT AIUIAPI aiui_db_long(AIUIDataBundle db, const char* key, long defaultVal);
const char AIUIEXPORT * AIUIAPI aiui_db_string(AIUIDataBundle db, const char* key, const char* defaultVal);
const char AIUIEXPORT * AIUIAPI aiui_db_binary(AIUIDataBundle db, const char* key, int* dataLen);

/*******************************AIUI_AIUIEvent*********************************/
typedef void* AIUIEvent;
typedef void (*AIUIMessageCallback)(const AIUIEvent ae, void *data);

int AIUIEXPORT AIUIAPI aiui_event_type(const AIUIEvent ae);
int AIUIEXPORT AIUIAPI aiui_event_arg1(const AIUIEvent ae);
int AIUIEXPORT AIUIAPI aiui_event_arg2(const AIUIEvent ae);
const char AIUIEXPORT * AIUIAPI aiui_event_info(const AIUIEvent ae);
AIUIDataBundle AIUIEXPORT AIUIAPI aiui_event_databundle(const AIUIEvent ae);

/**********************************AIUIBuffer***********************************/
typedef void* AIUIBuffer;

void AIUIEXPORT AIUIAPI aiui_buffer_destroy(AIUIBuffer ab);
AIUIBuffer AIUIEXPORT AIUIAPI aiui_create_buffer_from_data(const void* data, size_t len);
/******************************************************************************/

/**********************************AIUIMessage**********************************/
typedef void* AIUIMessage;

AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create(int msgType, int arg1 = 0, int arg2 = 0, const char* params = "", AIUIBuffer data = 0);
void AIUIEXPORT AIUIAPI aiui_msg_destroy(AIUIMessage msg);

/***********************************AIUI_Agent***********************************/
typedef void* AIUIAgent;
AIUIAgent AIUIEXPORT AIUIAPI aiui_agent_create(const char* params, AIUIMessageCallback callback, void *usrData);
void AIUIEXPORT AIUIAPI aiui_agent_send_message(AIUIAgent agent, AIUIMessage msg);
void AIUIEXPORT AIUIAPI aiui_agent_destroy(AIUIAgent agent);

/***********************************IUI_Setting**********************************/

enum LogLevel { _info, _debug, _warn, _error, _none };

enum VersionType {
    INTELLIGENT_HDW,    // 智能硬件版本
    MOBILE_PHONE,       // 移动互联网版本
    DESKTOP_PC          // 桌面PC版本
};

bool AIUIEXPORT AIUIAPI aiui_set_aiui_dir(const char* szDir);
const char AIUIEXPORT * AIUIAPI aiui_get_aiui_dir();

bool AIUIEXPORT AIUIAPI aiui_set_msc_dir(const char* szDir);
bool AIUIEXPORT AIUIAPI aiui_set_msc_cfg(const char* szCfg);
bool AIUIEXPORT AIUIAPI aiui_init_logger(const char* szLogDir = "");
void AIUIEXPORT AIUIAPI aiui_set_log_level(LogLevel level);
void AIUIEXPORT AIUIAPI aiui_set_net_log_level(LogLevel level);
void AIUIEXPORT AIUIAPI aiui_set_save_data_log(bool save, int logSizeMB = -1);
bool AIUIEXPORT AIUIAPI aiui_set_data_log_dir(const char* szDir);
bool AIUIEXPORT AIUIAPI aiui_set_raw_audio_dir(const char* dir);
bool AIUIEXPORT AIUIAPI aiui_is_mobile_version();
VersionType AIUIEXPORT AIUIAPI aiui_get_version_type();
void AIUIEXPORT AIUIAPI aiui_set_system_info(const char* key, const char* val);

#ifdef __cplusplus
};
#endif

/* clang-format on */

#endif    //AIUI_SRC_AIUI_C_H

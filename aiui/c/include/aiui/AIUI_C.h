/**
* AIUI_C.h
*
*  Created on: 2020年06月26日
*      Author: AIUI开放平台（https://aiui.xfyun.cn/）
*/
#ifndef AIUI_SRC_AIUI_C_H
#define AIUI_SRC_AIUI_C_H

#include <stddef.h>
#include "AIUICommon.h"

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

int AIUIEXPORT AIUIAPI aiui_strlen(const char * str);

/**********************************AIUIBuffer***********************************/
typedef void* AIUIBuffer;

void AIUIEXPORT AIUIAPI aiui_buffer_destroy(AIUIBuffer ab);
AIUIBuffer AIUIEXPORT AIUIAPI aiui_create_buffer_from_data(const void* data, size_t len);
/******************************************************************************/

/**********************************AIUIMessage**********************************/
typedef void* AIUIMessage;

AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create1(int msgType);
AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create2(int msgType, int arg1);
AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create3(int msgType, int arg1, int arg2);
AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create4(int msgType, int arg1, int arg2, const char* params);
AIUIMessage AIUIEXPORT AIUIAPI aiui_msg_create(int msgType, int arg1, int arg2, const char* params, AIUIBuffer data);

void AIUIEXPORT AIUIAPI aiui_msg_destroy(AIUIMessage msg);

/***********************************AIUI_Agent***********************************/
typedef void* AIUIAgent;
AIUIAgent AIUIEXPORT AIUIAPI aiui_agent_create(const char* params, AIUIMessageCallback callback, void *data);
void AIUIEXPORT AIUIAPI aiui_agent_send_message(AIUIAgent agent, AIUIMessage msg);
void AIUIEXPORT AIUIAPI aiui_agent_destroy(AIUIAgent agent);

/***********************************AIUI_Setting**********************************/

typedef enum { aiui_debug, aiui_info, aiui_warn, aiui_error, aiui_none } LogLevel;

typedef enum  {
   AIUI_INTELLIGENT_HDW,    /* 智能硬件版本 */
   AIUI_MOBILE_PHONE,       /* 移动互联网版本 */
   AIUI_DESKTOP_PC          /* 桌面PC版本 */
} VersionType;

int AIUIEXPORT AIUIAPI aiui_set_aiui_dir(const char* szDir);
const char AIUIEXPORT * AIUIAPI aiui_get_aiui_dir();

int AIUIEXPORT AIUIAPI aiui_set_msc_dir(const char* szDir);
int AIUIEXPORT AIUIAPI aiui_set_msc_cfg(const char* szCfg);
int AIUIEXPORT AIUIAPI aiui_init_logger(const char* szLogDir);
void AIUIEXPORT AIUIAPI aiui_set_log_level(LogLevel level);
void AIUIEXPORT AIUIAPI aiui_set_net_log_level(LogLevel level);
void AIUIEXPORT AIUIAPI aiui_set_save_data_log(int save, int logSizeMB);
int AIUIEXPORT AIUIAPI aiui_set_data_log_dir(const char* szDir);
int AIUIEXPORT AIUIAPI aiui_set_raw_audio_dir(const char* dir);
int AIUIEXPORT AIUIAPI aiui_is_mobile_version();
VersionType AIUIEXPORT AIUIAPI aiui_get_version_type();
void AIUIEXPORT AIUIAPI aiui_set_system_info(const char* key, const char* val);
int AIUIEXPORT AIUIAPI aiui_get_system_info(const char * key, char *out, int max_size);

#ifdef __cplusplus
};
#endif

/* clang-format on */

#endif    /* AIUI_SRC_AIUI_C_H */

/**
 * AIUI_V2.h
 *
 *  Created on: 2020年06月26日
 *      Author: AIUI开放平台（https://aiui.xfyun.cn/）
 */
#ifndef AIUI_SDK_AIUI_V2_H
#define AIUI_SDK_AIUI_V2_H

#include "AIUIConstant.h"
#include "AIUIErrorCode.h"
#include "AIUICommon.h"
#include "AIUI_C.h"

namespace aiui_v2 {
using namespace aiui;

void __AIUIMessageCallback(const AIUIEvent ae, void* data);

/**
 * 获取SDK版本信息。
 */
static inline const char* getVersion()
{
    return aiui_get_version();
}

/*
  数据捆绑类，支持int、long、string和Buffer*类型数据捆绑传输。
 */
class IDataBundle
{
    friend class IAIUIEvent;
    friend void __AIUIMessageCallback(const AIUIEvent ae, void* data);

private:
    AIUIDataBundle ab;

public:
    /**
	  注：getString返回地址对应的内存由IDataBundle管理，不可（不需要）在外部delete或者free。
	 **/
    inline const char* getString(const char* key, const char* defaultVal) const;

    /**
	  注：getBinary返回地址对应的内存由IDataBundle管理，不可（不需要）在外部delete或者free。
	 **/
    inline const char* getBinary(const char* key, int* dataLen) const;

    inline long getLong(const char* key, long defVal) const;

    inline int getInt(const char* key, int defVal) const;
};

/**
 * AIUI事件类。业务结果、SDK内部状态变化等输出信息都通过事件抛出。
 *
 * 注：SDK通过回调抛出事件后会立即释放事件中的内存对象，若要在别处使用到这些内存对象，开发者需要在回调中做拷贝。
 */
class IAIUIEvent
{
    friend void __AIUIMessageCallback(const AIUIEvent ae, void* data);

private:
    AIUIEvent ae;
    IDataBundle db;

public:
    /**
	 * 获取事件类型。取值参见AIUIConstant中EVENT_开头的常量定义。
	 */
    inline int getEventType() const;

    /**
	 * 获取扩展参数1。
	 */
    inline int getArg1() const;

    /**
	 * 获取扩展参数2。
	 */
    inline int getArg2() const;

    /**
	 * 获取描述信息。返回的指针不可外部delete或者free。
	 */
    inline const char* getInfo() const;

    /**
	 * 获取附带数据。返回的内存不可外部delete或者free。
	 */
    inline const IDataBundle* getData() const;
};

/**
 * AIUI消息类。AIUI所有的输入都是通过消息发送到SDK内部。
 */
class IAIUIMessage
{
    friend class IAIUIAgent;

private:
    AIUIMessage am;

public:
    /**
	 * 创建消息对象。
	 *
     * @param msgType 消息类型，参见AIUIConstant中CMD_开头的常量定义
	 * @param arg1 扩展参数1
	 * @param arg2 扩展参数2
	 * @param params 业务参数，传入后内部会做拷贝
     * @param data 附带数据，Message在SDK内部处理后会自动释放, 不需要外部释放
     *
	 * @return IAIUIMessage 对象指针
	 */
    inline static IAIUIMessage* create(
        int msgType, int arg1 = 0, int arg2 = 0, const char* params = "", AIUIBuffer data = 0);

    /**
	 * 销毁消息对象本身。
	 *
	 * 注：该方法不会释放消息体中的Buffer* data指针。
	 */
    inline void destroy();
};

/**
 * AIUI事件监听接口。SDK所有输出都通过回调onEvent方法抛出。
 */
class IAIUIListener
{
public:
    /**
	 * 事件回调，SDK所有输出都通过event抛出。
	 *
	 * @param event AIUI事件
	 */
    virtual void onEvent(const IAIUIEvent& event) = 0;
};

/**
 * AIUI代理类，单例对象，应用通过代理对象与AIUI交互。
 */
class IAIUIAgent
{
    friend void __AIUIMessageCallback(const AIUIEvent ae, void* data);

private:
    AIUIAgent ag;
    IAIUIListener* listener;

public:
    /**
	 * 创建Agent单例对象，SDK开始运行。
     *
	 * 注：此方法已被标注过时
	 * 注：该方法总是返回非空对象，非空并不代表创建过程中无错误发生。
	 *
	 * @param params 参数设置
	 * @param listener 事件监听器
	 * @return AIUIAgent 单例对象指针
	 */
    inline static IAIUIAgent AIUI_DEPRECATED* createAgent(const char* params, IAIUIListener* listener);

    /**
	 * 创建Agent单例对象，SDK开始运行。
	 *
	 * 注：该方法总是返回非空对象，非空并不代表创建过程中无错误发生。
	 *
	 * @param params 参数设置
	 * @param listener 事件监听器
     * @param sn 设备唯一ID
	 * @return AIUIAgent 单例对象指针
	 */
    inline static IAIUIAgent* createAgent(const char* params, IAIUIListener* listener, const char* sn);

    /**
	 * 发送消息给AIUI，消息中可以包含命令、参数和数据，具体格式参见IAIUIMessage。
	 *
	 * @param message AIUI消息，如果指定了非空的Buffer *data，在SDK内部处理后会自动release()这部分data，不需要外部去释放
	 */
    inline void sendMessage(const IAIUIMessage* message);

    /**
	 * 销毁Agent对象，SDK停止运行。
	 */
    inline void destroy();
};

/**
 * AIUI设置类，用于控制日志等设置。
 */
class AIUISetting
{
public:
    /**
	 * 设置AIUI工作目录，SDK会在该路径下保存日志、数据等文件。
	 *
	 * @param dir 路径（windows下以"\\"结尾，其他平台以"/"结尾），不能为空
	 * @return 是否设置成功
	 */
    inline static bool setAIUIDir(const char* szDir);

    /**
	 * 设置MSC工作目录，SDK会从该路径下读取基本配置，保存云端交互日志。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param dir 路径（windows下以"\\"结尾，其他平台以"/"结尾），不能为空
	 * @return 是否设置成功
	 */
    inline static bool setMscDir(const char* szDir);

    /**
	 * 设置msc.cfg的内容到SDK中。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param cfg msc.cfg中的实际内容
	 * @return 是否设置成功
	 */
    inline static bool setMscCfg(const char* szCfg);

    /**
	 * 初始化日志记录器，设置日志保存目录。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param logDir 日志目录（windows下以"\\"结尾，其他平台以"/"结尾），为空则在AIUI工作目录下创建log目录
	 * @return 是否成功
	 */
    inline static bool initLogger(const char* szLogDir = "");

    /**
	 * 设置上层调试日志打印级别，默认级别为info。
	 *
	 * 注：
	 * 		1.Android上通过logcat输出调试日志；
	 * 		2.iOS上通过控制台输出调试日志；
	 * 		3.Linux/Windows上暂无调试日志输出。
	 *
	 * @param level 日志等级，具体取值参考LogLevel定义
	 */
    inline static void setLogLevel(LogLevel level);

    /**
	 * 设置底层网络交互日志打印级别。
	 *
	 * 注：网络交互日志默认打印到aiui.log文件。
	 *
	 * @param level 日志等级，具体取值参考LogLevel定义
	 */
    inline static void setNetLogLevel(LogLevel level);

    /**
	 * 设置是否保存数据日志，即输入的音频和云端返回的结果。
	 *
	 * @param save 是否保存
	 * @param logSizeMB 日志大小
	 */
    inline static void setSaveDataLog(bool save, int logSizeMB = -1);

    /**
	 * 设置数据日志保存目录，不设置则默认是在AIUI工作目录下的data目录。
	 *
	 * @param dir 目录（windows下以"\\"结尾，其他平台以"/"结尾）
	 * @return 是否设置成功
	 */
    inline static bool setDataLogDir(const char* szDir);

    /**
	 * 设置原始音频保存目录，不设置则默认是在AIUI工作目录下的audio/raw/目录。
	 *
	 * @param dir 目录（windows下以"\\"结尾，其他平台以"/"结尾）
	 */
    inline static bool setRawAudioDir(const char* dir);

    /**
	 * 是否为移动互联网版本。
	 *
	 * @return true或者false
	 */
    inline static bool isMobileVersion();

    /**
	 * 获取版本类型。
	 *
	 * @return 参考VersionType定义
	 */
    inline static VersionType getVersionType();

    /**
	 * 设置系统信息，如sn等。
	 *
	 * @param key
	 * @param val
	 */
    inline static void setSystemInfo(const char* key, const char* val);

    /**
     * 获取系统信息，如sn等。
     *
     * @param key
     * @param out       存放获取结果的位置
     * @param max_size  存放结果的容量
     * @return size     若结果区的大小适合取出的数据返回 0，否在返回待取数据的长度
     *                  需再次准备好缓冲区再执行 getSystemInfo
     * 例如：
     *
     * char *sn = (char *)malloc(32);
     * memset(sn, '\0', 32);
     * int ret = AIUISetting::getSystemInfo("sn", sn, 32);
     * if(ret != 0) {
     *      free(sn);
     *      sn = (char *)malloc(ret);
     *      memset(sn, '\0', ret);
     *      AIUISetting::getSystemInfo("sn", sn, ret);
     * }
     *
     */
    inline static int getSystemInfo(const char* key, char* out, int max_size);
};

/**************************************************************************************************/
inline void __AIUIMessageCallback(const AIUIEvent ae, void* data)
{
    IAIUIAgent* agent = (IAIUIAgent*)data;
    IAIUIEvent e;
    e.ae = ae;
    e.db.ab = aiui_event_databundle(ae);
    agent->listener->onEvent(e);
}

inline IAIUIMessage* IAIUIMessage::create(
    int msgType, int arg1, int arg2, const char* params, AIUIBuffer data)
{
    IAIUIMessage* msg = new IAIUIMessage;
    msg->am = aiui_msg_create(msgType, arg1, arg2, params, data);
    return msg;
}
inline void IAIUIMessage::destroy()
{
    aiui_msg_destroy(am);
    delete this;
}

inline void IAIUIAgent::sendMessage(const IAIUIMessage* message)
{
    aiui_agent_send_message(ag, message->am);
}

inline IAIUIAgent* IAIUIAgent::createAgent(const char* params, IAIUIListener* listener)
{
    IAIUIAgent* agent = new IAIUIAgent;
    agent->listener = listener;
    agent->ag = aiui_agent_create(params, __AIUIMessageCallback, (void*)agent);

    return agent;
}

inline IAIUIAgent* IAIUIAgent::createAgent(const char* params,
                                           IAIUIListener* listener,
                                           const char* sn)
{
    aiui_set_system_info(AIUI_KEY_SERIAL_NUM, sn);

    return createAgent(params, listener);
}

inline void IAIUIAgent::destroy()
{
    aiui_agent_destroy(ag);
    delete this;
}

inline const char* IDataBundle::getString(const char* key, const char* defaultVal) const
{
    return aiui_db_string(ab, key, defaultVal);
}

inline const char* IDataBundle::getBinary(const char* key, int* dataLen) const
{
    return aiui_db_binary(ab, key, dataLen);
}

inline long IDataBundle::getLong(const char* key, long defVal) const
{
    return aiui_db_long(ab, key, defVal);
}

inline int IDataBundle::getInt(const char* key, int defVal) const
{
    return aiui_db_int(ab, key, defVal);
}

inline const IDataBundle* IAIUIEvent::getData() const
{
    return db.ab ? (const IDataBundle*)&db : NULL;
}

inline int IAIUIEvent::getEventType() const
{
    return aiui_event_type(ae);
}

inline int IAIUIEvent::getArg1() const
{
    return aiui_event_arg1(ae);
}

inline int IAIUIEvent::getArg2() const
{
    return aiui_event_arg2(ae);
}

inline const char* IAIUIEvent::getInfo() const
{
    return aiui_event_info(ae);
}

inline bool AIUISetting::setAIUIDir(const char* szDir)
{
    return aiui_set_aiui_dir(szDir);
}

inline VersionType AIUISetting::getVersionType()
{
    return aiui_get_version_type();
}

inline bool AIUISetting::isMobileVersion()
{
    return aiui_is_mobile_version();
}

inline bool AIUISetting::setRawAudioDir(const char* dir)
{
    return aiui_set_raw_audio_dir(dir);
}
inline bool AIUISetting::setDataLogDir(const char* szDir)
{
    return aiui_set_data_log_dir(szDir);
}

inline void AIUISetting::setSaveDataLog(bool save, int logSizeMB)
{
    aiui_set_save_data_log(save, logSizeMB);
}

inline void AIUISetting::setNetLogLevel(LogLevel level)
{
    aiui_set_net_log_level(level);
}

inline void AIUISetting::setLogLevel(LogLevel level)
{
    aiui_set_log_level(level);
}

inline bool AIUISetting::initLogger(const char* szLogDir)
{
    return aiui_init_logger(szLogDir);
}

inline bool AIUISetting::setMscCfg(const char* szCfg)
{
    return aiui_set_msc_cfg(szCfg);
}

inline bool AIUISetting::setMscDir(const char* szDir)
{
    return aiui_set_msc_dir(szDir);
}

inline void AIUISetting::setSystemInfo(const char* key, const char* val)
{
    aiui_set_system_info(key, val);
}

inline int AIUISetting::getSystemInfo(const char* key, char* out, int max_size)
{
    return aiui_get_system_info(key, out, max_size);
}

/**************************************************************************************************/

}    // namespace aiui_v2

#endif    //AIUI_SDK_AIUI_V2_H

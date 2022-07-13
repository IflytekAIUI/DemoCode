/*
 * 	AIUI.h
 *
 *  Created on: 2017年2月17日
 *      Author: AIUI开放平台（https://aiui.xfyun.cn/）
 */

#ifndef AIUI_API_H_
#define AIUI_API_H_

#include "AIUIConstant.h"
#include "AIUIErrorCode.h"
#include "AIUIType.h"
#include "AIUICommon.h"

namespace aiui {

/**
 * 获取SDK版本信息。
 */
AIUIEXPORT const char* getVersion();

/**
 * AIUI事件类。业务结果、SDK内部状态变化等输出信息都通过事件抛出。
 *
 * 注：SDK通过回调抛出事件后会立即释放事件中的内存对象，若要在别处使用到这些内存对象，开发者需要在回调中做拷贝。
 */
class IAIUIEvent
{
public:
    AIUIEXPORT virtual ~IAIUIEvent();

    /**
	 * 获取事件类型。取值参见AIUIConstant中EVENT_开头的常量定义。
	 */
    virtual int getEventType() const = 0;

    /**
	 * 获取扩展参数1。
	 */
    virtual int getArg1() const = 0;

    /**
	 * 获取扩展参数2。
	 */
    virtual int getArg2() const = 0;

    /**
	 * 获取描述信息。返回的指针不可外部delete或者free。
	 */
    virtual const char* getInfo() const = 0;

    /**
	 * 获取附带数据。返回的内存不可外部delete或者free。
	 */
    virtual IDataBundle* getData() const = 0;
};

/**
 * AIUI消息类。AIUI所有的输入都是通过消息发送到SDK内部。
 */
class IAIUIMessage
{
public:
    AIUIEXPORT virtual ~IAIUIMessage();

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
    AIUIEXPORT static IAIUIMessage* create(
        int msgType, int arg1 = 0, int arg2 = 0, const char* params = "", Buffer* data = 0);

    /**
	 * 获取消息类型。类型取值参见AIUIConstant中CMD_开头的常量定义。
	 */
    virtual int getMsgType() const = 0;

    /**
	 * 获取扩展参数1。
	 */
    virtual int getArg1() const = 0;

    /**
	 * 获取扩展参数2。
	 */
    virtual int getArg2() const = 0;

    /**
	 * 获取业务参数。
	 */
    virtual const char* getParams() const = 0;

    /**
	 * 获取附带数据。
	 */
    virtual Buffer* getData() const = 0;

    /**
	 * 释放附带数据。
	 */
    virtual void releaseData() = 0;

    /**
	 * 销毁消息对象本身。
	 *
	 * 注：该方法不会释放消息体中的Buffer* data指针。
	 */
    virtual void destroy() = 0;
};

/**
 * AIUI事件监听接口。SDK所有输出都通过回调onEvent方法抛出。
 */
#ifdef AIUI_WINDOWS
class AIUIListener
#else
class AIUIEXPORT AIUIListener
#endif
{
public:
    AIUIEXPORT virtual ~AIUIListener();

    /**
	 * 事件回调，SDK所有输出都通过event抛出。
	 *
	 * @param event AIUI事件
	 */
    AIUIEXPORT virtual void onEvent(const IAIUIEvent& event) const = 0;
};

typedef AIUIListener IAIUIListener;

/**
 * AIUI代理类，单例对象，应用通过代理对象与AIUI交互。
 */
class IAIUIAgent
{
public:
    AIUIEXPORT virtual ~IAIUIAgent();

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
    AIUIEXPORT AIUI_DEPRECATED static IAIUIAgent *createAgent(const char* params, const IAIUIListener* listener);

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
    AIUIEXPORT static IAIUIAgent* createAgent(const char* params, const IAIUIListener* listener, const char *sn);

    /**
	 * 发送消息给AIUI，消息中可以包含命令、参数和数据，具体格式参见IAIUIMessage。
	 *
	 * @param message AIUI消息，如果指定了非空的Buffer *data，在SDK内部处理后会自动release()这部分data，不需要外部去释放
	 */
    virtual void sendMessage(const IAIUIMessage* message) = 0;

    /**
	 * 销毁Agent对象，SDK停止运行。
	 */
    virtual void destroy() = 0;
};

/**
 * 日志级别。
 */
enum LogLevel { _debug, _info, _warn, _error, _none };

/**
 * 版本类型。
 */
enum VersionType {
    INTELLIGENT_HDW,    // 智能硬件版本
    MOBILE_PHONE,       // 移动互联网版本
    DESKTOP_PC          // 桌面PC版本
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
    AIUIEXPORT static bool setAIUIDir(const char* szDir);

    /**
	 * 设置MSC工作目录，SDK会从该路径下读取基本配置，保存云端交互日志。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param dir 路径（windows下以"\\"结尾，其他平台以"/"结尾），不能为空
	 * @return 是否设置成功
	 */
    AIUIEXPORT static bool setMscDir(const char* szDir);

    /**
	 * 设置msc.cfg的内容到SDK中。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param cfg msc.cfg中的实际内容
	 * @return 是否设置成功
	 */
    AIUIEXPORT static bool setMscCfg(const char* szCfg);

    /**
	 * 初始化日志记录器，设置日志保存目录。
	 *
	 * 注：需要在createAgent之前调用，否则无效。
	 *
	 * @param logDir 日志目录（windows下以"\\"结尾，其他平台以"/"结尾），为空则在AIUI工作目录下创建log目录
	 * @return 是否成功
	 */
    AIUIEXPORT static bool initLogger(const char* szLogDir = "");

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
    AIUIEXPORT static void setLogLevel(LogLevel level);

    /**
	 * 设置底层网络交互日志打印级别。
	 *
	 * 注：网络交互日志默认打印到aiui.log文件。
	 *
	 * @param level 日志等级，具体取值参考LogLevel定义
	 */
    AIUIEXPORT static void setNetLogLevel(LogLevel level);

    /**
	 * 设置是否保存数据日志，即输入的音频和云端返回的结果。
	 *
	 * @param save 是否保存
	 * @param logSizeMB 日志大小
	 */
    AIUIEXPORT static void setSaveDataLog(bool save, int logSizeMB = -1);

    /**
	 * 设置数据日志保存目录，不设置则默认是在AIUI工作目录下的data目录。
	 *
	 * @param dir 目录（windows下以"\\"结尾，其他平台以"/"结尾）
	 * @return 是否设置成功
	 */
    AIUIEXPORT static bool setDataLogDir(const char* szDir);

    /**
	 * 设置原始音频保存目录，不设置则默认是在AIUI工作目录下的audio/raw/目录。
	 *
	 * @param dir 目录（windows下以"\\"结尾，其他平台以"/"结尾）
	 */
    AIUIEXPORT static bool setRawAudioDir(const char* dir);

    /**
	 * 是否为移动互联网版本。
	 *
	 * @return true或者false
	 */
    AIUIEXPORT static bool isMobileVersion();

    /**
	 * 获取版本类型。
	 *
	 * @return 参考VersionType定义
	 */
    AIUIEXPORT static VersionType getVersionType();

    /**
	 * 设置系统信息，如sn等。
	 *
	 * @param key
	 * @param val
	 */
    AIUIEXPORT static void setSystemInfo(const char* key, const char* val);

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
     * char *sn = (char *)malloc(1024);
     * memset(sn, '\0', 1024);
     * int ret = AIUISetting::getSystemInfo("sn", sn, 1024);
     * if(ret != 0) {
     *      free(sn);
     *      sn = (char *)malloc(ret);
     *      memset(sn, '\0', ret);
     *      AIUISetting::getSystemInfo("sn", sn, ret);
     * }
     *
	 */
    AIUIEXPORT static int getSystemInfo(const char* key, char* out, int max_size);
};

AIUIEXPORT const char* getAIUIDir();

}    // namespace aiui

#ifdef AIUI_LIB_COMPILING
#    include "aiui_internal/AIUI_internal.h"
#endif

#endif /* AIUI_API_H_ */

#!/bin/env python3
# -*- coding: UTF-8 -*-

import ctypes
import os
import functools
from enum import IntEnum, unique

aiui = ctypes.windll.LoadLibrary(os.getcwd() + "/aiui.dll")


class AIUIConstant(IntEnum):
    """aiui相关的常量"""

    """
    结果事件。info为结果描述JSON字符串，data为结果数据map，通过解析params
    可以获取结果类型和结果的key值，然后用key值从data获取结果。
    """
    EVENT_RESULT = 1
    """错误事件。arg1为错误码，info为错误描述信息。"""
    EVENT_ERROR = 2
    """状态事件。arg1为服务状态，取值：STATE_IDLE、STATE_READY、STATE_WORKING。"""
    EVENT_STATE = 3
    """
    唤醒事件。info为唤醒结果JSON字符串，示例：
    {
      "power": 12342435436,   // 唤醒能量值
      "beam":3,               // 拾音波束号，唤醒成功后阵列将在该波束方向上拾音
      "angle":180,            // 唤醒角度
      "channel":5,            // 唤醒声道，即麦克风编号，表示该声道的音频质量最好
      "CMScore":132           // 声道对应的唤醒得分
    }
    """
    EVENT_WAKEUP = 4
    """
    休眠事件。当出现交互超时，或者外部发送CMD_RESET_WAKEUP消息重置唤醒时抛出该事件。
    arg1取值：TYPE_AUTO（自动休眠）、TYPE_COMPEL（外部强制休眠，发送CMD_RESET_WAKEUP）。
    """
    EVENT_SLEEP = 5
    """
    VAD事件。arg1表示VAD消息类型，取值：VAD_BOS，VAD_VOL，VAD_EOS。
    当arg1为VAD_VOL时，arg2表示音量，取值范围：[0-30]。
    """
    EVENT_VAD = 6
    """绑定成功事件，当ServiceKit与服务绑定成功时抛出。Linux版本无此事件。"""
    EVENT_BIND_SUCESS = 7
    """
    CMD对应的返回事件。arg1表示对应的CMD命令（如CMD_BUILD_GRAMMAR等），arg2为返回码（0表示成功），
    info为返回的结果。
    """
    EVENT_CMD_RETURN = 8
    """音频抛出事件。16k音频数据在data中，通过键"audio"获取。"""
    EVENT_AUDIO = 9
    """准备休眠事件。抛出该事件通知外部即将休眠，若10秒之内无有效交互再抛出EVENT_SLEEP。"""
    EVENT_PRE_SLEEP = 10
    """开始录音事件。通知外部录音开始，用户可以开始说话。"""
    EVENT_START_RECORD = 11
    """停止录音事件。"""
    EVENT_STOP_RECORD = 12
    """与服务端建立起连接事件，建立连接后才能进行如数据同步等各种操作。"""
    EVENT_CONNECTED_TO_SERVER = 13
    """与服务端断开连接事件。"""
    EVENT_SERVER_DISCONNECTED = 14
    """TTS对外抛出事件。"""
    EVENT_TTS = 15
    """CAE鉴权抛出来的明文。"""
    EVENT_CAE_PLAIN_TEXT = 1000
    """推送消息事件。"""
    EVENT_PUSH_MESSAGE = 1001

    """ 空闲状态，AIUI服务未开启。 """
    STATE_IDLE = 1
    """ 就绪状态，已经开启录音，等待唤醒。"""
    STATE_READY = 2
    """ 工作状态，已经唤醒，可以开始人机交互。"""
    STATE_WORKING = 3

    """获取AIUI服务状态。发送该命令后，AIUI会回应EVENT_STATE事件。"""
    CMD_GET_STATE = 1
    """写入数据。params为描述数据信息（数据类型等）的JSON字符串，data携带二进制数据。"""
    CMD_WRITE = 2
    """停止写入。params为描述数据信息（数据类型等）的JSON字符串"""
    CMD_STOP_WRITE = 3
    """重启服务。相当于停止后再启动服务，重启时会重新加载配置以使新配置生效。"""
    CMD_RESET = 4
    """开启服务。用于在服务停止时启动服务，启动时会重新加载配置以使新配置生效。"""
    CMD_START = 5
    """停止服务。用于在服务启动时停止服务，停止后不再接收输入。"""
    CMD_STOP = 6
    """
    唤醒命令，用于外部主动唤醒。arg1字段为麦克风阵列波束编号，默认为0号波束，唤醒
    后会向该波束方向拾音。唤醒成功后，会抛出EVENT_WAKEUP事件。
    """
    CMD_WAKEUP = 7
    """重置唤醒。将AIUI重置为待唤醒状态，由STATE_WORKING变为STATE_READY。"""
    CMD_RESET_WAKEUP = 8
    """设置阵列拾音波束。arg1为波束编号，编号随阵列类型的不同而不同，设置后会向该波束方向拾音。"""
    CMD_SET_BEAM = 9
    """设置参数。params为aiui.cfg文件内容，即一个JSON字符串。"""
    CMD_SET_PARAMS = 10
    """     
    上传用户词表。params为JSON字符串，示例：
     {
       "name":"userword",   // 词表名称
       "content":"XXXX"     // 词表内容
     }
     其中XXXX也为一个JSON字符串，示例：
     {
       "userword":[
        {
           "name":"我的常用词",
           "words":["佳晨实业","蜀南庭苑","高兰路","复联二"]
        },
        {
           "name":"我的好友",
           "words":["李馨琪","鹿晓雷","张集栋","周家莉","叶震珂","熊泽萌"]
        }]
    }
    """
    CMD_UPLOAD_LEXICON = 11
    """上传日志。params为JSON格式的日志。"""
    CMD_SEND_LOG = 12
    """同步操作。"""
    CMD_SYNC = 13
    """
    开始保存数据。params为键值对参数，用于指明数据类型，示例：
    *data_type = raw_audio，暂时只支持原始音频一种类型的数据。
    """
    CMD_START_SAVE = 14
    """
    停止保存数据。params为键值对参数，用于指明数据类型，示例：
    data_type="raw_audio"，暂时只支持原始音频一种类型的数据。
    """
    CMD_STOP_SAVE = 15
    """构建识别语法。params为bnf语法内容。"""
    CMD_BUILD_GRAMMAR = 16
    """
    更新本地词典，必须在调用过CMD_BUILD_GRAMMAR之后调用。
    params为JSON字符串，用于指定词表名称和内容，示例：
    {
      "name":"<contact>",  // 词表名称
      "content":"张三\n李四\n" //词表内容
    }
     """
    CMD_UPDATE_LOCAL_LEXICON = 17
    """
        开始抛出识别音频。调用之后，抛出EVENT_AUDIO事件。
        arg1为拾音波束编号设置，若当前未唤醒则会使用该波束拾音，若已经处于唤醒状态则arg1不起作用。
         """
    CMD_START_THROW_AUDIO = 18
    """
    停止抛出识别音频。调用之后，停止抛出EVENT_AUDIO事件。
     """
    CMD_STOP_THROW_AUDIO = 19
    """
    确认云端返回结果的有效性，用于延后休眠，在得到结果之后5秒时间内发送有效。
    """
    CMD_RESULT_VALIDATION_ACK = 20
    """
    清除云端语义对话历史。
    """
    CMD_CLEAN_DIALOG_HISTORY = 21
    """
    开始录制，需要在params中通过data_type字段指定录制的数据类型。
    """
    CMD_START_RECORD = 22
    """
    停止录制，需要在params中通过data_type字段指定停止录制的数据类型。
    """
    CMD_STOP_RECORD = 23
    """
    查询数据同步状态，需要在params中通过sid字段指定CMD_SYNC返回的sid。
    """
    CMD_QUERY_SYNC_STATUS = 24
    """
    查询参数。params为当前AIUI配置生效参数。
    """
    CMD_QUERY_PARAMS = 25
    """
    动态设置CAE参数。
    """
    CMD_SET_CAE_PARAMS = 26
    """
    TTS合成命令，arg1表示具体的操作，取值：START, PAUSE, RESUME, CANCEL。
    """
    CMD_TTS = 27
    """
    加载编译好的语法
    arg1 表示加载的语法ID
     """
    CMD_LOAD_GRAMMAR = 28
    """
    卸载语法
    arg1 b表示卸载的语法ID
     """
    CMD_UNLOAD_GRAMMAR = 29
    """
    CAE鉴权，写入设备信息。
    """
    CMD_CAE_WRITE_DEVINFO = 1000
    """
    CAE鉴权，写入加密后的密文。
    """
    CMD_CAE_WRITE_ENCRYPT = 1001
    """ 操作成功。 """
    SUCCESS = 0
    """操作失败。"""
    FAIL = -1
    """ VAD消息类型，前端点。 """
    VAD_BOS = 0
    """ VAD消息类型，音量。 """
    VAD_VOL = 1
    """VAD消息类型，后端点。"""
    VAD_EOS = 2
    """VAD消息类型，前端点超时。"""
    VAD_BOS_TIMEOUT = 3
    """由SDK内部自动抛出的事件。"""
    TYPE_AUTO = 0
    """由外部操作导致抛出的事件。"""
    TYPE_COMPEL = 1
    """同步状态数据。"""
    SYNC_DATA_STATUS = 0
    """ 同步个性化数据。"""
    SYNC_DATA_INDIVIDUAL = 1
    """第三方账号关联数据。"""
    SYNC_DATA_ACCOUNT = 2
    """同步雅典娜个性化数据。
    """
    SYNC_DATA_SCHEMA = 3
    """查询数据同步状态。 """
    SYNC_DATA_QUERY = 4
    """所见即可说数据。"""
    SYNC_DATA_SPEAKABLE = 5

    START = 1
    PAUSE = 2
    RESUME = 3
    CANCEL = 4


def aiui_get_version():
    _f = aiui.aiui_get_version
    _f.restype = ctypes.c_char_p
    s = _f()
    return str(s, encoding="utf-8")


def aiui_init_cae_engine(mic_type: str):
    _f = aiui.aiui_init_cae_engine
    _f.restype = ctypes.c_int
    _f.argtypes = [ctypes.c_char_p]

    return int(_f(ctypes.c_char_p(mic_type.encode('utf-8'))))


def aiui_destroy_cae_engine():
    _f = aiui.aiui_destroy_cae_engine
    _f.restype = ctypes.c_int

    return int(_f())


class IDataBundle:
    aiui_db = None

    def __init__(self, aiui_db: ctypes.c_void_p):
        self.aiui_db = aiui_db

        self.aiui_db_int = aiui.aiui_db_int
        self.aiui_db_int.restype = ctypes.c_int
        self.aiui_db_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]

        self.aiui_db_long = aiui.aiui_db_long
        self.aiui_db_long.restype = ctypes.c_long
        self.aiui_db_long.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]

        self.aiui_db_string = aiui.aiui_db_string
        self.aiui_db_string.restype = ctypes.c_char_p
        self.aiui_db_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]

        self.aiui_db_binary = aiui.aiui_db_binary
        self.aiui_db_binary.restype = ctypes.c_void_p
        self.aiui_db_binary.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]

    def __bool__(self):
        return self.aiui_db is not None

    def getInt(self, key: str, defaultVal: int):
        return int(self.aiui_db_int(self.aiui_db, ctypes.c_char_p(key.encode('utf-8')),
                                    ctypes.pointer(ctypes.c_int(defaultVal))))

    def getLong(self, key: str, defaultVal: int):
        return int(self.aiui_db_long(self.aiui_db, ctypes.c_char_p(key.encode('utf-8')),
                                     ctypes.pointer(ctypes.c_long(defaultVal))))

    def getString(self, key: str, defaultVal: str):
        s = self.aiui_db_string(self.aiui_db, ctypes.c_char_p(key.encode('utf-8')),
                                ctypes.c_char_p(defaultVal.encode('utf-8')))
        return str(s, encoding="utf-8")

    def getBinary(self, key: str):
        datalen = ctypes.c_int(0)

        s = self.aiui_db_binary(self.aiui_db, ctypes.c_char_p(key.encode('utf-8')), ctypes.pointer(datalen))

        arrayType = ctypes.c_byte * datalen.value
        pa = ctypes.cast(s, ctypes.POINTER(arrayType))

        return pa.contents


class IAIUIEvent:
    aiui_event = None

    def __init__(self, aiui_event):
        self.aiui_event = aiui_event

        self.aiui_event_type = aiui.aiui_event_type
        self.aiui_event_type.restype = ctypes.c_int
        self.aiui_event_type.argtypes = [ctypes.c_void_p]

        self.aiui_event_arg1 = aiui.aiui_event_arg1
        self.aiui_event_arg1.restype = ctypes.c_int
        self.aiui_event_arg1.argtypes = [ctypes.c_void_p]

        self.aiui_event_arg2 = aiui.aiui_event_arg2
        self.aiui_event_arg2.restype = ctypes.c_int
        self.aiui_event_arg2.argtypes = [ctypes.c_void_p]

        self.aiui_event_info = aiui.aiui_event_info
        self.aiui_event_info.restype = ctypes.c_char_p
        self.aiui_event_info.argtypes = [ctypes.c_void_p]

        self.aiui_event_databundle = aiui.aiui_event_databundle
        self.aiui_event_databundle.restype = ctypes.c_void_p
        self.aiui_event_databundle.argtypes = [ctypes.c_void_p]

    def getEventType(self) -> int:
        return self.aiui_event_type(self.aiui_event)

    def getArg1(self) -> int:
        return self.aiui_event_arg1(self.aiui_event)

    def getArg2(self) -> int:
        return self.aiui_event_arg2(self.aiui_event)

    def getInfo(self) -> str:
        s = self.aiui_event_info(self.aiui_event)
        return str(s, encoding="utf-8")

    def getData(self) -> IDataBundle:
        db = self.aiui_event_databundle(self.aiui_event)
        return IDataBundle(db)


class Buffer:
    aiui_buf = None

    def __init__(self, aiui_buf):
        self.aiui_buf = aiui_buf

    @staticmethod
    def create(dataBytearray: bytes):
        _f = aiui.aiui_create_buffer_from_data
        _f.restype = ctypes.c_void_p
        _f.argtypes = [ctypes.c_char_p, ctypes.c_size_t]

        return Buffer(_f(ctypes.c_char_p(dataBytearray), ctypes.c_size_t(len(dataBytearray))))


class IAIUIMessage:
    aiui_msg = None

    def __init__(self, aiui_msg):
        self.aiui_msg = aiui_msg

    @staticmethod
    def create(msgType: AIUIConstant, arg1=0, arg2=0, params="", data=Buffer(None)):
        _f = aiui.aiui_msg_create
        _f.restype = ctypes.c_void_p
        _f.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_void_p]

        return IAIUIMessage(
            _f(ctypes.c_int(msgType.value), ctypes.c_int(arg1), ctypes.c_int(arg2),
               ctypes.c_char_p(params.encode('utf-8')),
               data.aiui_buf))

    def destroy(self):
        _f = aiui.aiui_msg_destroy
        _f.argtypes = [ctypes.c_void_p]

        return _f(self.aiui_msg)


AIUIListenerCallback = None


def EventCallback(func):
    @functools.wraps(func)
    def wrapper(ev: ctypes.c_void_p):
        return func(IAIUIEvent(ev))

    return wrapper


class IAIUIAgent:
    aiui_agent = None

    def __init__(self, aiui_agent):
        self.aiui_agent = aiui_agent

        self.aiui_agent_send_message = aiui.aiui_agent_send_message
        self.aiui_agent_send_message.argtypes = [ctypes.c_void_p, ctypes.c_void_p]

        self.aiui_agent_destroy = aiui.aiui_agent_destroy

    def sendMessage(self, msg: IAIUIMessage):
        return self.aiui_agent_send_message(self.aiui_agent, msg.aiui_msg)

    def destroy(self):
        return self.aiui_agent_destroy(self.aiui_agent)

    @staticmethod
    def createAgent(params: str, listener):
        _f = aiui.aiui_agent_create
        _f.argtypes = [ctypes.c_char_p, ctypes.c_void_p]
        _f.restype = ctypes.c_void_p

        global AIUIListenerCallback
        AIUIListenerCallback = ctypes.CFUNCTYPE(None, ctypes.c_void_p)(listener)

        return IAIUIAgent(_f(ctypes.c_char_p(params.encode('utf-8')), AIUIListenerCallback))


@unique
class LogLevel(IntEnum):
    """日志等级"""

    Info = 0
    Debug = 1
    Warn = 2
    Error = 3
    DiableAll = 4


@unique
class VersionType(IntEnum):
    """SDK版本信息"""

    """智能硬件版本"""
    INTELLIGENT_HDW = 0
    """移动互联网版本"""
    MOBILE_PHONE = 1
    """桌面PC版本"""
    DESKTOP_PC = 2


class AIUISetting:
    @staticmethod
    def setAIUIDir(szDir: str):
        _f = aiui.aiui_set_aiui_dir
        _f.restype = ctypes.c_bool
        _f.argtypes = [ctypes.c_char_p]

        return _f(ctypes.c_char_p(szDir.encode('utf-8')))

    @staticmethod
    def getAIUIDir():
        _f = aiui.aiui_get_aiui_dir
        _f.restype = ctypes.c_char_p

        s = _f()

        return str(s, encoding="utf-8")

    @staticmethod
    def setMscDir(szDir: str):
        _f = aiui.aiui_set_msc_dir
        _f.restype = ctypes.c_bool
        _f.argtypes = [ctypes.c_char_p]

        return _f(ctypes.c_char_p(szDir.encode('utf-8')))

    @staticmethod
    def setMscCfg(szDir: str):
        _f = aiui.aiui_set_msc_cfg
        _f.restype = ctypes.c_bool
        _f.argtypes = [ctypes.c_char_p]

        return _f(ctypes.c_char_p(szDir.encode('utf-8')))

    @staticmethod
    def initLogger(szDir: str):
        _f = aiui.aiui_init_logger
        _f.restype = ctypes.c_bool
        _f.argtypes = [ctypes.c_char_p]

        return _f(ctypes.c_char_p(szDir.encode('utf-8')))

    @staticmethod
    def setNetLogLevel(level: LogLevel):
        _f = aiui.aiui_set_net_log_level
        _f.argtypes = [ctypes.c_int]

        return _f(level.value)

    @staticmethod
    def setLogLevel(level: LogLevel):
        _f = aiui.aiui_set_log_level
        _f.argtypes = [ctypes.c_int]

        return _f(level.value)

    @staticmethod
    def setSaveDataLog(save: bool, logSizeMB: int = -1):
        _f = aiui.aiui_set_save_data_log
        _f.argtypes = [ctypes.c_bool, ctypes.c_int]

        return _f(ctypes.c_bool(save), ctypes.c_int(logSizeMB))

    @staticmethod
    def setRawAudioDir(szDir: str):
        _f = aiui.aiui_set_raw_audio_dir
        _f.restype = ctypes.c_bool
        _f.argtypes = [ctypes.c_char_p]

        return _f(ctypes.c_char_p(szDir.encode('utf-8')))

    @staticmethod
    def isMobileVersion() -> bool:
        _f = aiui.aiui_is_mobile_version
        _f.restype = ctypes.c_bool

        return _f()

    @staticmethod
    def getVersionType() -> VersionType:
        _f = aiui.aiui_get_version_type
        _f.restype = ctypes.c_int

        return VersionType(_f())

    @staticmethod
    def setSystemInfo(key: str, val: str):
        _f = aiui.aiui_set_system_info
        _f.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

        return _f(ctypes.c_char_p(key.encode('utf-8')), ctypes.c_char_p(val.encode('utf-8')))

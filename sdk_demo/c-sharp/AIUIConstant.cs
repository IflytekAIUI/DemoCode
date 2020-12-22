﻿namespace turing_csharp_demo
{
    class AIUIConstant
    {
        /*******************事件类型**********************/
        /**
        * 结果事件。info为结果描述JSON字符串，data为结果数据map，通过解析params
        * 可以获取结果类型和结果的key值，然后用key值从data获取结果。
        **/
        public const int EVENT_RESULT = 1;

        /**
        * 错误事件。arg1为错误码，info为错误描述信息。
        **/
        public const int EVENT_ERROR = 2;

        /**
        * 状态事件。arg1为服务状态，取值：STATE_IDLE、STATE_READY、STATE_WORKING。
        **/
        public const int EVENT_STATE = 3;

        /**
        * 唤醒事件。info为唤醒结果JSON字符串，示例：<br/>
        * <pre>{
        *   "power": 12342435436,   // 唤醒能量值
        *   "beam":3,               // 拾音波束号，唤醒成功后阵列将在该波束方向上拾音
        *   "angle":180,            // 唤醒角度
        *   "channel":5,            // 唤醒声道，即麦克风编号，表示该声道的音频质量最好
        *   "CMScore":132           // 声道对应的唤醒得分
        * }</pre>
        **/
        public const int EVENT_WAKEUP = 4;

        /**
        * 休眠事件。当出现交互超时，或者外部发送CMD_RESET_WAKEUP消息重置唤醒时抛出该事件。
        * arg1取值：TYPE_AUTO（自动休眠）、TYPE_COMPEL（外部强制休眠，发送CMD_RESET_WAKEUP）。
        **/
        public const int EVENT_SLEEP = 5;

        /**
        * VAD事件。arg1表示VAD消息类型，取值：VAD_BOS，VAD_VOL，VAD_EOS。
        * 当arg1为VAD_VOL时，arg2表示音量，取值范围：[0-30]。
*/
        public const int EVENT_VAD = 6;

        /**
        * 绑定成功事件，当ServiceKit与服务绑定成功时抛出。Linux版本无此事件。
*/
        public const int EVENT_BIND_SUCESS = 7;

        /**
        * CMD对应的返回事件。arg1表示对应的CMD命令（如CMD_BUILD_GRAMMAR等），arg2为返回码（0表示成功），
        * info为返回的结果。
*/
        public const int EVENT_CMD_RETURN = 8;

        /**
        * 音频抛出事件。16k音频数据在data中，通过键"audio"获取。
*/
        public const int EVENT_AUDIO = 9;

        /**
        * 准备休眠事件。抛出该事件通知外部即将休眠，若10秒之内无有效交互再抛出EVENT_SLEEP。
*/
        public const int EVENT_PRE_SLEEP = 10;

        /**
        * 开始录音事件。通知外部录音开始，用户可以开始说话。
*/
        public const int EVENT_START_RECORD = 11;

        /**
        * 停止录音事件。
*/
        public const int EVENT_STOP_RECORD = 12;

        /**
        * 与服务端建立起连接事件，建立连接后才能进行如数据同步等各种操作。
*/
        public const int EVENT_CONNECTED_TO_SERVER = 13;

        /**
        * 与服务端断开连接事件。
*/
        public const int EVENT_SERVER_DISCONNECTED = 14;

        /**
        * TTS对外抛出事件。
*/
        public const int EVENT_TTS = 15;

        /**
        * CAE鉴权抛出来的明文。
*/
        public const int EVENT_CAE_PLAIN_TEXT = 1000;

        /**
        * 推送消息事件。
*/
        public const int EVENT_PUSH_MESSAGE = 1001;

        /*******************运行状态**********************/
        /**
        * 空闲状态，AIUI服务未开启。
        **/
        public const int STATE_IDLE = 1;

        /**
        * 就绪状态，已经开启录音，等待唤醒。
        **/
        public const int STATE_READY = 2;

        /**
        * 工作状态，已经唤醒，可以开始人机交互。
        **/
        public const int STATE_WORKING = 3;

        /*******************命令定义**********************/
        /**
        * 获取AIUI服务状态。发送该命令后，AIUI会回应EVENT_STATE事件。
        **/
        public const int CMD_GET_STATE = 1;

        /**
        * 写入数据。params为描述数据信息（数据类型等）的JSON字符串，data携带二进制数据。
        **/
        public const int CMD_WRITE = 2;

        /**
        * 停止写入。params为描述数据信息（数据类型等）的JSON字符串。
        **/
        public const int CMD_STOP_WRITE = 3;

        /**
        * 重启服务。相当于停止后再启动服务，重启时会重新加载配置以使新配置生效。
        **/
        public const int CMD_RESET = 4;

        /**
        * 开启服务。用于在服务停止时启动服务，启动时会重新加载配置以使新配置生效。
        **/
        public const int CMD_START = 5;

        /**
        * 停止服务。用于在服务启动时停止服务，停止后不再接收输入。
        **/
        public const int CMD_STOP = 6;

        /**
        * 唤醒命令，用于外部主动唤醒。arg1字段为麦克风阵列波束编号，默认为0号波束，唤醒
        * 后会向该波束方向拾音。唤醒成功后，会抛出EVENT_WAKEUP事件。
        **/
        public const int CMD_WAKEUP = 7;

        /**
        * 重置唤醒。将AIUI重置为待唤醒状态，由STATE_WORKING变为STATE_READY。
        **/
        public const int CMD_RESET_WAKEUP = 8;

        /**
        * 设置阵列拾音波束。arg1为波束编号，编号随阵列类型的不同而不同，设置后会向该波束方向拾音。
        **/
        public const int CMD_SET_BEAM = 9;

        /**
        * 设置参数。params为aiui.cfg文件内容，即一个JSON字符串。
        **/
        public const int CMD_SET_PARAMS = 10;

        /**
        * 上传用户词表。params为JSON字符串，示例：
        * <pre>{
        *   "name":"userword",   // 词表名称
        *   "content":"XXXX"     // 词表内容
        * }</pre>
        * 其中XXXX也为一个JSON字符串，示例：
        * <pre>{
        *   "userword":[
        *    {
        *       "name":"我的常用词",
        *       "words":["佳晨实业","蜀南庭苑","高兰路","复联二"]
        *    },
        *    {
        *       "name":"我的好友",
        *       "words":["李馨琪","鹿晓雷","张集栋","周家莉","叶震珂","熊泽萌"]
        *    }]
        * }</pre>
        **/
        public const int CMD_UPLOAD_LEXICON = 11;

        /**
        * 上传日志。params为JSON格式的日志。
        **/
        public const int CMD_END_LOG = 12;

        /**
        * 同步操作。
        **/
        public const int CMD_SYNC = 13;

        /**
        * 开始保存数据。params为键值对参数，用于指明数据类型，示例：
        * data_type=raw_audio，暂时只支持原始音频一种类型的数据。
        **/
        public const int CMD_START_SAVE = 14;

        /**
        * 停止保存数据。params为键值对参数，用于指明数据类型，示例：
        * data_type="raw_audio"，暂时只支持原始音频一种类型的数据。
        **/
        public const int CMD_STOP_SAVE = 15;

        /**
        * 构建识别语法。params为bnf语法内容。
        **/
        public const int CMD_BUILD_GRAMMAR = 16;

        /**
        * 更新本地词典，必须在调用过CMD_BUILD_GRAMMAR之后调用。
        * params为JSON字符串，用于指定词表名称和内容，示例：<pre>
        * {
        *   "name":"<contact>",  // 词表名称
        *   "content":"张三\n李四\n" //词表内容
        * }<pre>
        **/
        public const int CMD_UPDATE_LOCAL_LEXICON = 17;

        /**
        * 开始抛出识别音频。调用之后，抛出EVENT_AUDIO事件。
        * arg1为拾音波束编号设置，若当前未唤醒则会使用该波束拾音，若已经处于唤醒状态则arg1不起作用。
        **/
        public const int CMD_START_THROW_AUDIO = 18;

        /**
        * 停止抛出识别音频。调用之后，停止抛出EVENT_AUDIO事件。
        **/
        public const int CMD_STOP_THROW_AUDIO = 19;

        /**
        * 确认云端返回结果的有效性，用于延后休眠，在得到结果之后5秒时间内发送有效。
*/
        public const int CMD_RESULT_VALIDATION_ACK = 20;

        /**
        * 清除云端语义对话历史。
*/
        public const int CMD_CLEAN_DIALOG_HISTORY = 21;

        /**
        * 开始录制，需要在params中通过data_type字段指定录制的数据类型。
*/
        public const int CMD_START_RECORD = 22;

        /**
        * 停止录制，需要在params中通过data_type字段指定停止录制的数据类型。
*/
        public const int CMD_STOP_RECORD = 23;

        /**
        * 查询数据同步状态，需要在params中通过sid字段指定CMD_SYNC返回的sid。
*/
        public const int CMD_QUERY_SYNC_STATUS = 24;

        /**
        * 查询参数。params为当前AIUI配置生效参数。
*/
        public const int CMD_QUERY_PARAMS = 25;

        /**
        * 动态设置CAE参数。
*/
        public const int CMD_SET_CAE_PARAMS = 26;

        /**
        * TTS合成命令，arg1表示具体的操作，取值：START = 1, CANCEL = 4。
*/
        public const int CMD_TTS = 27;

        /**
        * CAE鉴权，写入设备信息。
*/
        public const int CMD_CAE_WRITE_DEVINFO = 1000;

        /**
        * CAE鉴权，写入加密后的密文。
*/
        public const int CMD_CAE_WRITE_ENCRYPT = 1001;

        /*******************错误码定义**********************/
        /**
         * 操作成功。
         */
        public const int SUCCESS = 0;

        public const int NLP_SUCCESS_NO_DATA = 3;
        /**
         * 操作失败。
         */
        public const int FAIL = -1;

        /*******************其他常量**********************/
        /**
         * VAD消息类型，前端点。
         */
        public const int VAD_BOS = 0;

        /**
         * VAD消息类型，音量。
         */
        public const int VAD_VOL = 1;

        /**
         * VAD消息类型，后端点。
         */
        public const int VAD_EOS = 2;

        /**
         * VAD消息类型，前端点超时。
         */
        public const int VAD_BOS_TIMEOUT = 3;

        /**
         * 由SDK内部自动抛出的事件。
         */
        public const int TYPE_AUTO = 0;

        /**
         * 由外部操作导致抛出的事件。
         */
        public const int TYPE_COMPEL = 1;

        public const int START = 1;

        public const int PAUSE = 2;

        public const int RESUME = 3;

        public const int CANCEL = 4;

    }
}

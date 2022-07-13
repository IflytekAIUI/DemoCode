/*
 * AIUIErrorCode.h
 *
 *  Created on: 2017年2月17日
 *      Author: AIUI开放平台（https://aiui.xfyun.cn/）
 */

#ifndef AIUIERRORCODE_H_
#define AIUIERRORCODE_H_

#include "AIUICommon.h"

namespace aiui {
/* clang-format off */
class AIUIErrorCode
{
public:
    /*
     * 10100 - 19999错误码
     * 1开头的错误码由MSC上报出来，产生自MSC/MSS/引擎
     * 分析相关错误码产生原因可以借助msc.log和结构化日志.
     */
    static const int SUCCESS                                    =  0;
    static const int FAIL                                       = -1;

    static const int MSP_SUCCESS                               =  0;
    static const int MSP_ERROR_FAIL                            = -1;
    static const int MSP_ERROR_EXCEPTION                       = -2;

    /* General errors 10100(0x2774) */
    static const int MSP_ERROR_GENERAL                         = 10100;     /* 0x2774 */
    static const int MSP_ERROR_OUT_OF_MEMORY                   = 10101;     /* 0x2775 */
    static const int MSP_ERROR_FILE_NOT_FOUND                  = 10102;     /* 0x2776 */
    static const int MSP_ERROR_NOT_SUPPORT                     = 10103;     /* 0x2777 */
    static const int MSP_ERROR_NOT_IMPLEMENT                   = 10104;     /* 0x2778 */
    static const int MSP_ERROR_ACCESS                          = 10105;     /* 0x2779 */
    static const int MSP_ERROR_INVALID_PARA                    = 10106;     /* 0x277A */  /* 缺少参数 */
    static const int MSP_ERROR_INVALID_PARA_VALUE              = 10107;     /* 0x277B */  /* 无效参数值 */
    static const int MSP_ERROR_INVALID_HANDLE                  = 10108;     /* 0x277C */
    static const int MSP_ERROR_INVALID_DATA                    = 10109;     /* 0x277D */
    static const int MSP_ERROR_NO_LICENSE                      = 10110;     /* 0x277E */  /* 引擎授权不足 */
    static const int MSP_ERROR_NOT_INIT                        = 10111;     /* 0x277F */  /* 引擎未初始化,可能是引擎崩溃 */
    static const int MSP_ERROR_NULL_HANDLE                     = 10112;     /* 0x2780 */
    static const int MSP_ERROR_OVERFLOW                        = 10113;     /* 0x2781 */  /* 单用户下模型数超上限(10个); */
    /* 只出现在测试时对一个用户进行并发注册 */
    static const int MSP_ERROR_TIME_OUT                        = 10114;     /* 0x2782 */  /* 超时 */
    static const int MSP_ERROR_OPEN_FILE                       = 10115;     /* 0x2783 */
    static const int MSP_ERROR_NOT_FOUND                       = 10116;     /* 0x2784 */  /* 数据库中模型不存在 */
    static const int MSP_ERROR_NO_ENOUGH_BUFFER                = 10117;     /* 0x2785 */
    static const int MSP_ERROR_NO_DATA                         = 10118;     /* 0x2786 */  /* 从客户端读音频或从引擎段获取结果时无数据 */
    static const int MSP_ERROR_NO_MORE_DATA                    = 10119;     /* 0x2787 */
    static const int MSP_ERROR_NO_RESPONSE_DATA                = 10120;     /* 0x2788 */
    static const int MSP_ERROR_ALREADY_EXIST                   = 10121;     /* 0x2789 */  /* 数据库中模型已存在 */
    static const int MSP_ERROR_LOAD_MODULE                     = 10122;     /* 0x278A */
    static const int MSP_ERROR_BUSY                            = 10123;     /* 0x278B */
    static const int MSP_ERROR_INVALID_CONFIG                  = 10124;     /* 0x278C */
    static const int MSP_ERROR_VERSION_CHECK                   = 10125;     /* 0x278D */
    static const int MSP_ERROR_CANCELED                        = 10126;     /* 0x278E */
    static const int MSP_ERROR_INVALID_MEDIA_TYPE              = 10127;     /* 0x278F */
    static const int MSP_ERROR_CONFIG_INITIALIZE               = 10128;     /* 0x2790 */
    static const int MSP_ERROR_CREATE_HANDLE                   = 10129;     /* 0x2791 */
    static const int MSP_ERROR_CODING_LIB_NOT_LOAD             = 10130;     /* 0x2792 */
    static const int MSP_ERROR_USER_CANCELLED                  = 10131;     /* 0x2793 */
    static const int MSP_ERROR_INVALID_OPERATION               = 10132;     /* 0x2794 */
    static const int MSP_ERROR_MESSAGE_NOT_COMPLETE            = 10133;     /* 0x2795 */   /* flash */
    static const int MSP_ERROR_NO_EID                          = 10134;     /* 0x2795 */
    static const int MSP_ERROE_OVER_REQ                        = 10135;     /* 0x2797 */   /* client Redundancy request */
    static const int MSP_ERROR_USER_ACTIVE_ABORT               = 10136;     /* 0x2798 */   /* user abort */
    static const int MSP_ERROR_BUSY_GRMBUILDING                = 10137;     /* 0x2799 */
    static const int MSP_ERROR_BUSY_LEXUPDATING                = 10138;     /* 0x279A */
    static const int MSP_ERROR_SESSION_RESET                   = 10139;     /* 0x279B */   /* msc主动终止会话，准备重传 */
    static const int MSP_ERROR_BOS_TIMEOUT                     = 10140;     /* 0x279C */   /* VAD前端点超时 */
    static const int MSP_ERROR_STREAM_FILTER                   = 10141;     /* 0X279D */   /* AIUI当前Stream被过滤 */
    static const int MSP_ERROR_STREAM_CLEAR                    = 10142;     /* 0X279E */   /* AIUI当前Stream被清理 */
    static const int MSP_ERROR_NLP_TIMEOUT                     = 10146;     /* 语义结果超时 */
    static const int MSP_ERROR_TPP_TIMEOUT                     = 10147;     /* 后处理调用超时 */

    /* Error codes of network 10200(0x27D8)*/
    static const int MSP_ERROR_NET_GENERAL                      = 10200;     /* 0x27D8 */
    static const int MSP_ERROR_NET_OPENSOCK                     = 10201;     /* 0x27D9 */   /* Open socket */
    static const int MSP_ERROR_NET_CONNECTSOCK                  = 10202;     /* 0x27DA */   /* Connect socket */
    static const int MSP_ERROR_NET_ACCEPTSOCK                   = 10203;     /* 0x27DB */   /* Accept socket */
    static const int MSP_ERROR_NET_SENDSOCK                     = 10204;     /* 0x27DC */   /* Send socket data */
    static const int MSP_ERROR_NET_RECVSOCK                     = 10205;     /* 0x27DD */   /* Recv socket data */
    static const int MSP_ERROR_NET_INVALIDSOCK                  = 10206;     /* 0x27DE */   /* Invalid socket handle */
    static const int MSP_ERROR_NET_BADADDRESS                   = 10207;     /* 0x27EF */   /* Bad network address */
    static const int MSP_ERROR_NET_BINDSEQUENCE                 = 10208;     /* 0x27E0 */   /* Bind after listen/connect */
    static const int MSP_ERROR_NET_NOTOPENSOCK                  = 10209;     /* 0x27E1 */   /* Socket is not opened */
    static const int MSP_ERROR_NET_NOTBIND                      = 10210;     /* 0x27E2 */   /* Socket is not bind to an address */
    static const int MSP_ERROR_NET_NOTLISTEN                    = 10211;     /* 0x27E3 */   /* Socket is not listening */
    static const int MSP_ERROR_NET_CONNECTCLOSE                 = 10212;     /* 0x27E4 */   /* The other side of connection is closed */
    static const int MSP_ERROR_NET_NOTDGRAMSOCK                 = 10213;     /* 0x27E5 */   /* The socket is not datagram type */
    static const int MSP_ERROR_NET_DNS                          = 10214;     /* 0x27E6 */   /* domain name is invalid or dns server does not function well */
    static const int MSP_ERROR_NET_INIT                         = 10215;     /* 0x27E7 */   /* ssl ctx create failed */

    /*nfl error*/
    static const int MSP_ERROR_NFL_INNER_ERROR                  = 10216;   /* NFL inner error */
    static const int MSP_ERROR_MSS_TIME_OUT                     = 10217;   /* MSS TIMEOUT */
    static const int MSP_ERROT_CLIENT_TIME_OUT                  = 10218;   /* CLIENT TIMEOUT */
    static const int MSP_ERROR_CLIENT_CLOSE                     = 10219;   /* CLIENT CLOSED CONNECTION */

    static const int MSP_ERROR_CLIENT_AREA_CHANGE               = 10220;
    static const int MSP_ERROR_NET_SSL_HANDSHAKE                = 10221;
    static const int MSP_ERROR_NET_INVALID_ROOT_CERT            = 10222;
    static const int MSP_ERROR_NET_INVALID_CLIENT_CERT          = 10223;
    static const int MSP_ERROR_NET_INVALID_SERVER_CERT          = 10224;
    static const int MSP_ERROR_NET_INVALID_KEY                  = 10225;


    /* Error codes of mssp message 10300(0x283C) */
    static const int MSP_ERROR_MSG_GENERAL                     = 10300;     /* 0x283C */
    static const int MSP_ERROR_MSG_PARSE_ERROR                 = 10301;     /* 0x283D */
    static const int MSP_ERROR_MSG_BUILD_ERROR                 = 10302;     /* 0x283E */
    static const int MSP_ERROR_MSG_PARAM_ERROR                 = 10303;     /* 0x283F */
    static const int MSP_ERROR_MSG_CONTENT_EMPTY               = 10304;     /* 0x2840 */
    static const int MSP_ERROR_MSG_INVALID_CONTENT_TYPE        = 10305;     /* 0x2841 */
    static const int MSP_ERROR_MSG_INVALID_CONTENT_LENGTH      = 10306;     /* 0x2842 */
    static const int MSP_ERROR_MSG_INVALID_CONTENT_ENCODE      = 10307;     /* 0x2843 */
    static const int MSP_ERROR_MSG_INVALID_KEY                 = 10308;     /* 0x2844 */
    static const int MSP_ERROR_MSG_KEY_EMPTY                   = 10309;     /* 0x2845 */
    static const int MSP_ERROR_MSG_SESSION_ID_EMPTY            = 10310;     /* 0x2846 */
    static const int MSP_ERROR_MSG_LOGIN_ID_EMPTY              = 10311;     /* 0x2847 */
    static const int MSP_ERROR_MSG_SYNC_ID_EMPTY               = 10312;     /* 0x2848 */
    static const int MSP_ERROR_MSG_APP_ID_EMPTY                = 10313;     /* 0x2849 */
    static const int MSP_ERROR_MSG_EXTERN_ID_EMPTY             = 10314;     /* 0x284A */
    static const int MSP_ERROR_MSG_INVALID_CMD                 = 10315;     /* 0x284B */
    static const int MSP_ERROR_MSG_INVALID_SUBJECT             = 10316;     /* 0x284C */
    static const int MSP_ERROR_MSG_INVALID_VERSION             = 10317;     /* 0x284D */
    static const int MSP_ERROR_MSG_NO_CMD                      = 10318;     /* 0x284E */
    static const int MSP_ERROR_MSG_NO_SUBJECT                  = 10319;     /* 0x284F */
    static const int MSP_ERROR_MSG_NO_VERSION                  = 10320;     /* 0x2850 */
    static const int MSP_ERROR_MSG_MSSP_EMPTY                  = 10321;     /* 0x2851 */
    static const int MSP_ERROR_MSG_NEW_RESPONSE                = 10322;     /* 0x2852 */
    static const int MSP_ERROR_MSG_NEW_CONTENT                 = 10323;     /* 0x2853 */
    static const int MSP_ERROR_MSG_INVALID_SESSION_ID          = 10324;     /* 0x2854 */
    static const int MSP_ERROR_MSG_INVALID_CONTENT             = 10325;     /* 0x2855 */

    /* Error codes of DataBase 10400(0x28A0)*/
    static const int MSP_ERROR_DB_GENERAL                    = 10400;     /* 0x28A0 */
    static const int MSP_ERROR_DB_EXCEPTION                  = 10401;     /* 0x28A1 */
    static const int MSP_ERROR_DB_NO_RESULT                  = 10402;     /* 0x28A2 */
    static const int MSP_ERROR_DB_INVALID_USER               = 10403;     /* 0x28A3 */
    static const int MSP_ERROR_DB_INVALID_PWD                = 10404;     /* 0x28A4 */
    static const int MSP_ERROR_DB_CONNECT                    = 10405;     /* 0x28A5 */
    static const int MSP_ERROR_DB_INVALID_SQL                = 10406;     /* 0x28A6 */
    static const int MSP_ERROR_DB_INVALID_APPID              = 10407;    /* 0x28A7 */
    static const int MSP_ERROR_DB_NO_UID                     = 10408;


    /* Error codes of Resource 10500(0x2904)*/
    static const int MSP_ERROR_RES_GENERAL                   = 10500;     /* 0x2904 */
    static const int MSP_ERROR_RES_LOAD                      = 10501;     /* 0x2905 */   /* Load resource */
    static const int MSP_ERROR_RES_FREE                      = 10502;     /* 0x2906 */   /* Free resource */
    static const int MSP_ERROR_RES_MISSING                   = 10503;     /* 0x2907 */   /* Resource File Missing */
    static const int MSP_ERROR_RES_INVALID_NAME              = 10504;     /* 0x2908 */   /* Invalid resource file name */
    static const int MSP_ERROR_RES_INVALID_ID                = 10505;     /* 0x2909 */   /* Invalid resource ID */
    static const int MSP_ERROR_RES_INVALID_IMG               = 10506;     /* 0x290A */   /* Invalid resource image pointer */
    static const int MSP_ERROR_RES_WRITE                     = 10507;     /* 0x290B */   /* Write read-only resource */
    static const int MSP_ERROR_RES_LEAK                      = 10508;     /* 0x290C */   /* Resource leak out */
    static const int MSP_ERROR_RES_HEAD                      = 10509;     /* 0x290D */   /* Resource head currupt */
    static const int MSP_ERROR_RES_DATA                      = 10510;     /* 0x290E */   /* Resource data currupt */
    static const int MSP_ERROR_RES_SKIP                      = 10511;     /* 0x290F */   /* Resource file skipped */

    /* Error codes of TTS 10600(0x2968)*/
    static const int MSP_ERROR_TTS_GENERAL                   = 10600;     /* 0x2968 */
    static const int MSP_ERROR_TTS_TEXTEND                   = 10601;     /* 0x2969 */  /* Meet text end */
    static const int MSP_ERROR_TTS_TEXT_EMPTY                = 10602;     /* 0x296A */  /* no synth text */
    static const int MSP_ERROR_TTS_LTTS_ERROR                = 10603;     /* 0x296B */

    /* Error codes of Recognizer 10700(0x29CC) */
    static const int MSP_ERROR_REC_GENERAL                   = 10700;     /* 0x29CC */
    static const int MSP_ERROR_REC_INACTIVE                  = 10701;     /* 0x29CD */
    static const int MSP_ERROR_REC_GRAMMAR_ERROR             = 10702;     /* 0x29CE */
    static const int MSP_ERROR_REC_NO_ACTIVE_GRAMMARS        = 10703;     /* 0x29CF */
    static const int MSP_ERROR_REC_DUPLICATE_GRAMMAR         = 10704;     /* 0x29D0 */
    static const int MSP_ERROR_REC_INVALID_MEDIA_TYPE        = 10705;     /* 0x29D1 */
    static const int MSP_ERROR_REC_INVALID_LANGUAGE          = 10706;     /* 0x29D2 */
    static const int MSP_ERROR_REC_URI_NOT_FOUND             = 10707;     /* 0x29D3 */
    static const int MSP_ERROR_REC_URI_TIMEOUT               = 10708;     /* 0x29D4 */
    static const int MSP_ERROR_REC_URI_FETCH_ERROR           = 10709;     /* 0x29D5 */
    static const int MSP_ERROR_REC_PROC_MOD                  = 10710;     /* 0x29D6 */

    /* Error codes of Speech Detector 10800(0x2A30) */
    static const int MSP_ERROR_EP_GENERAL                    = 10800;     /* 0x2A30 */
    static const int MSP_ERROR_EP_NO_SESSION_NAME            = 10801;     /* 0x2A31 */
    static const int MSP_ERROR_EP_INACTIVE                   = 10802;     /* 0x2A32 */
    static const int MSP_ERROR_EP_INITIALIZED                = 10803;     /* 0x2A33 */

    /* Error codes of TUV */
    static const int MSP_ERROR_TUV_GENERAL                    = 10900;     /* 0x2A94 */
    static const int MSP_ERROR_TUV_GETHIDPARAM                = 10901;     /* 0x2A95 */   /* Get Busin Param huanid*/
    static const int MSP_ERROR_TUV_TOKEN                      = 10902;     /* 0x2A96 */   /* Get Token */
    static const int MSP_ERROR_TUV_CFGFILE                    = 10903;     /* 0x2A97 */   /* Open cfg file */
    static const int MSP_ERROR_TUV_RECV_CONTENT               = 10904;     /* 0x2A98 */   /* received content is error */
    static const int MSP_ERROR_TUV_VERFAIL                    = 10905;     /* 0x2A99 */   /* Verify failure */

    /* Error codes of IMTV */
    static const int MSP_ERROR_LOGIN_SUCCESS                  = 11000;     /* 0x2AF8 */   /* 成功 */
    static const int MSP_ERROR_LOGIN_NO_LICENSE               = 11001;     /* 0x2AF9 */   /* 试用次数结束，用户需要付费 */
    static const int MSP_ERROR_LOGIN_SESSIONID_INVALID        = 11002;     /* 0x2AFA */   /* SessionId失效，需要重新登录通行证 */
    static const int MSP_ERROR_LOGIN_SESSIONID_ERROR          = 11003;     /* 0x2AFB */   /* SessionId为空，或者非法 */
    static const int MSP_ERROR_LOGIN_UNLOGIN                  = 11004;     /* 0x2AFC */   /* 未登录通行证 */
    static const int MSP_ERROR_LOGIN_INVALID_USER             = 11005;     /* 0x2AFD */   /* 用户ID无效 */
    static const int MSP_ERROR_LOGIN_INVALID_PWD              = 11006;     /* 0x2AFE */   /* 用户密码无效 */
    static const int MSP_ERROR_LOGIN_SYSTEM_ERROR             = 11099;     /* 0x2B5B */   /* 系统错误 */

    /* Error codes of HCR */
    static const int MSP_ERROR_HCR_GENERAL                   = 11100;
    static const int MSP_ERROR_HCR_RESOURCE_NOT_EXIST        = 11101;
    static const int MSP_ERROR_HCR_CREATE                    = 11102;
    static const int MSP_ERROR_HCR_DESTROY                   = 11103;
    static const int MSP_ERROR_HCR_START                     = 11104;
    static const int MSP_ERROR_HCR_APPEND_STROKES            = 11105;
    static const int MSP_ERROR_HCR_INIT                      = 11106;
    static const int MSP_ERROR_HCR_POINT_DECODE              = 11107;
    static const int MSP_ERROR_HCR_DISPATCH                  = 11108;
    static const int MSP_ERROR_HCR_GETRESULT                 = 11109;
    static const int MSP_ERROR_HCR_RESOURCE                  = 11110;

    /* Error Codes using in local engine */
    static const int MSP_ERROR_AUTH_NO_LICENSE               = 11200;    /* 0x2BC0 */   /* 无授权 */
    static const int MSP_ERROR_AUTH_NO_ENOUGH_LICENSE        = 11201;    /* 0x2BC1 */   /* 授权不足 */
    static const int MSP_ERROR_AUTH_INVALID_LICENSE          = 11202;    /* 0x2BC2 */   /* 无效的授权 */
    static const int MSP_ERROR_AUTH_LICENSE_EXPIRED          = 11203;    /* 0x2BC3 */   /* 授权过期 */
    static const int MSP_ERROR_AUTH_NEED_MORE_DATA           = 11204;    /* 0x2BC4 */   /* 无设备信息 */
    static const int MSP_ERROR_AUTH_LICENSE_TO_BE_EXPIRED    = 11205;    /* 0x2BC5 */   /* 授权即将过期，警告性错误码 */
    static const int MSP_ERROR_AUTH_INVALID_MACHINE_ID       = 11206;    /* 0x2BC6 */   /* 无效的机器码 */
    //黑名单禁止使用
    static const int MSP_ERROR_AUTH_LOCAL_ASR_FORBIDDEN       = 11207;    /* 0x2BC7 */   /* 禁止使用本地识别引擎 */
    static const int MSP_ERROR_AUTH_LOCAL_TTS_FORBIDDEN       = 11208;    /* 0x2BC8 */   /* 禁止使用本地合成引擎 */
    static const int MSP_ERROR_AUTH_LOCAL_IVW_FORBIDDEN       = 11209;     /* 0x2BC8 */   /* 禁止使用本地唤醒引擎 */

    static const int MSP_ERROR_AUTH_APPID_NOT_MATCH           = 11210;    /* 资源appid和应用appid不匹配 */
    static const int MSP_ERROR_AUTH_UID_NOT_MATCH             = 11211;    /* 资源uid和登录用户uid不匹配 */
    static const int MSP_ERROR_AUTH_TRIAL_EXPIRED             = 11212;    /* 试用资源过期 */
    static const int MSP_ERROR_AUTH_ERROR_END                 = 11250;     /* 0x2BC8 */   /* End 授权错误码*/

    static const int MSP_ERROR_AIUI_NO_ENOUGH_LICENSE         = 11216;    /* AIUI授权不足 */

    /*Error Codes of Authorization*/
    static const int MSP_ERROR_AUTH_DVC_NO_LICENSE               = 11300;
    static const int MSP_ERROR_AUTH_DVC_NO_ENOUGH_LICENSE        = 11301;
    static const int MSP_ERROR_AUTH_DVC_INVALID_LICENSE          = 11302;
    static const int MSP_ERROR_AUTH_DVC_LICENSE_EXPIRED          = 11303;
    static const int MSP_ERROR_AUTH_DVC_NEED_MORE_DATA           = 11304;
    static const int MSP_ERROR_AUTH_DVC_LICENSE_TO_BE_EXPIRED    = 11305;
    static const int MSP_ERROR_AUTH_DVC_EXCEED_LICENSE           = 11306;

    /*Error codes of ISE */
    static const int MSP_ERROR_ASE_EXCEP_SILENCE              = 11401;
    static const int MSP_ERROR_ASE_EXCEP_SNRATIO              = 11402;
    static const int MSP_ERROR_ASE_EXCEP_PAPERDATA            = 11403;
    static const int MSP_ERROR_ASE_EXCEP_PAPERCONTENTS        = 11404;
    static const int MSP_ERROR_ASE_EXCEP_NOTMONO              = 11405;
    static const int MSP_ERROR_ASE_EXCEP_OTHERS               = 11406;
    static const int MSP_ERROR_ASE_EXCEP_PAPERFMT             = 11407;
    static const int MSP_ERROR_ASE_EXCEP_ULISTWORD            = 11408;

    /*Error Code Of ivw optimization */
    static const int MSP_ERROR_IVW_MODEL_TRAINING             = 11501;  /* 模型资源正在生成中 */
    static const int MSP_ERROR_IVW_MODEL_NO_FOUND             = 11502;  /* 无新模型资源 */
    static const int MSP_ERROR_IVW_BUSY                       = 11503;  /* 服务繁忙 */

    //-------------------------声纹-------------------------//

    static const int MSP_ERROR_IVP_GENERAL                     = 11600; /** 内核异常*/
    static const int MSP_ERROR_IVP_EXTRA_RGN_SOPPORT           = 11601; /** rgn超过最大支持次数9*/
    static const int MSP_ERROR_IVP_TRUNCATED                   = 11602; /** 音频波形幅度太大，超出系统范围，发生截幅*/
    static const int MSP_ERROR_IVP_MUCH_NOISE                  = 11603; /** 太多噪音*/
    static const int MSP_ERROR_IVP_TOO_LOW                     = 11604; /** 声音太小*/
    static const int MSP_ERROR_IVP_ZERO_AUDIO                  = 11605; /** 没检测到音频*/
    static const int MSP_ERROR_IVP_UTTER_TOO_SHORT             = 11606; /** 音频太短*/
    static const int MSP_ERROR_IVP_TEXT_NOT_MATCH              = 11607; /** 音频内容与给定文本不一致*/
    static const int MSP_ERROR_IVP_NO_ENOUGH_AUDIO             = 11608; /** 音频长达不到自由说的要求*/
    static const int MSP_ERROR_IVP_MODEL_NOT_FOUND_IN_HBASE    = 11610; /** 声纹模型数据在hbase中找不到 **/
    static const int MSP_MODEL_NEED_UPDATE                     = 10031; /** 云端声纹模型正在更新,5s左右 **/
    //-------------------------声纹-------------------------//

    //-------------------------人脸-------------------------//

    static const int MSP_ERROR_IFR_NOT_FACE_IMAGE                    = 11700; /** 无人脸 */
    static const int MSP_ERROR_FACE_IMAGE_FULL_LEFT                  = 11701; /** 向左 */
    static const int MSP_ERROR_FACE_IMAGE_FULL_RIGHT                 = 11702; /** 向右 */
    static const int MSP_ERROR_IMAGE_CLOCKWISE_WHIRL                 = 11703; /** 顺时针旋转 */
    static const int MSP_ERROR_IMAGE_COUNTET_CLOCKWISE_WHIRL         = 11704; /** 逆时针旋转 */
    static const int MSP_ERROR_VALID_IMAGE_SIZE                      = 11705; /** 尺寸错误*/
    static const int MSP_ERROR_ILLUMINATION                          = 11706; /** 光照异常 */
    static const int MSP_ERROR_FACE_OCCULTATION                      = 11707; /** 人脸被遮挡 */
    static const int MSP_ERROR_FACE_INVALID_MODEL                    = 11708; /** 非法模型数据 */
    static const int MSP_ERROR_FACE_MODEL_NOT_FOUND_IN_HBASE         = 11712; /** 人脸模型数据在hbase中找不到 **/
    //-------------------------人脸-------------------------//

    //-------------------------融合验证-------------------------//
    static const int MSP_ERROR_FUSION_INVALID_INPUT_TYPE                = 11709; /** 输入数据类型非法 */
    static const int MSP_ERROR_FUSION_NO_ENOUGH_DATA                    = 11710; /** 输入的数据不完整 */
    static const int MSP_ERROR_FUSION_ENOUGH_DATA                       = 11711; /** 输入的数据过多 */
    //-------------------------融合验证-------------------------//

    /*Error Codes of AIUI*/
    static const int MSP_ERROR_AIUI_CID_EXPIRED				            = 11800;

    //-------------------------鉴别组-------------------------//
    static const int MSP_ERROR_NO_GROUP                               = 10143; /** 组不存在，未创建 */
    static const int MSP_ERROR_GROUP_EMPTY                            = 10141; /** 空组 */
    static const int MSP_ERROR_NO_USER                                = 10142; /** 组内没有该用户 */

    static const int MSP_ERROR_OVERFLOW_IN_GROUP                      = 10144; /** 用户数量超过组上限或者组创建数量超过上限 */
    //-------------------------鉴别组-------------------------//

    /* Error codes of http 12000(0x2EE0) */
    static const int MSP_ERROR_HTTP_BASE                     = 12000;    /* 0x2EE0 */

    /*Error codes of ISV */
    static const int MSP_ERROR_ISV_NO_USER                   = 13000;    /* 32C8 */    /* the user doesn't exist */

    /* Error codes of Lua scripts */
    static const int MSP_ERROR_LUA_BASE                      = 14000;    /* 0x36B0 */
    static const int MSP_ERROR_LUA_YIELD                     = 14001;    /* 0x36B1 */
    static const int MSP_ERROR_LUA_ERRRUN                    = 14002;    /* 0x36B2 */
    static const int MSP_ERROR_LUA_ERRSYNTAX                 = 14003;    /* 0x36B3 */
    static const int MSP_ERROR_LUA_ERRMEM                    = 14004;    /* 0x36B4 */
    static const int MSP_ERROR_LUA_ERRERR                    = 14005;    /* 0x36B5 */
    static const int MSP_ERROR_LUA_INVALID_PARAM             = 14006;    /* 0x36B6 */

    /* Error codes of MMP */
    static const int MSP_ERROR_MMP_BASE                      = 15000;    /* 0x3A98 */
    static const int MSP_ERROR_MMP_MYSQL_INITFAIL            = 15001;    /* 0x3A99 */
    static const int MSP_ERROR_MMP_REDIS_INITFAIL            = 15002;    /* 0x3A9A */
    static const int MSP_ERROR_MMP_NETDSS_INITFAIL           = 15003;    /* 0x3A9B */
    static const int MSP_ERROR_MMP_TAIR_INITFAIL             = 15004;    /* 0x3A9C */
    static const int MSP_ERROR_MMP_MAIL_SESSION_FAIL         = 15006;    /* 0x3A9E */    /* 邮件登陆服务器时，会话错误。*/
    static const int MSP_ERROR_MMP_MAIL_LOGON_FAIL           = 15007;    /* 0x3A9F */    /* 邮件登陆服务器时，拒绝登陆。*/
    static const int MSP_ERROR_MMP_MAIL_USER_ILLEGAL         = 15008;    /* 0x3AA0 */    /* 邮件登陆服务器时，用户名非法。*/
    static const int MSP_ERROR_MMP_MAIL_PWD_ERR              = 15009;    /* 0x3AA1 */    /* 邮件登陆服务器时，密码错误。*/
    static const int MSP_ERROR_MMP_MAIL_SOCKET_ERR           = 15010;    /* 0x3AA2 */    /* 邮件发送过程中套接字错误*/
    static const int MSP_ERROR_MMP_MAIL_INIT_FAIL            = 15011;    /* 0x3AA3 */    /* 邮件初始化错误*/
    static const int MSP_ERROR_MMP_STORE_MNR_NO_INIT         = 15012;    /* 0x3AA4 */    /* store_manager未初始化，或初始化失败*/
    static const int MSP_ERROR_MMP_STORE_MNR_POOL_FULL       = 15013;    /* 0x3AA5 */    /* store_manager的连接池满了*/
    static const int MSP_ERROR_MMP_STRATGY_PARAM_ILLEGAL     = 15014;    /* 0x3AA6 */    /* 报警策略表达式非法*/
    static const int MSP_ERROR_MMP_STRATGY_PARAM_TOOLOOG     = 15015;    /* 0x3AA7 */    /* 报警策略表达式太长*/
    static const int MSP_ERROR_MMP_PARAM_NULL                = 15016;    /* 0x3AA8 */    /* 函数参数为空*/
    static const int MSP_ERROR_MMP_ERR_MORE_TOTAL            = 15017;    /* 0x3AA9 */    /* pms插入数据库中错误汇总表的数据，错误次数 > 总次数。*/
    static const int MSP_ERROR_MMP_PROC_THRESHOLD            = 15018;    /* 0x3AAA */    /* 进程监控阀值设置错误*/
    static const int MSP_ERROR_MMP_SERVER_THRESHOLD          = 15019;    /* 0x3AAB */    /* 服务器监控阀值设置错误*/
    static const int MSP_ERROR_MMP_PYTHON_NO_EXIST           = 15020;    /* 0x3AAC */    /* python脚本文件不存在 */
    static const int MSP_ERROR_MMP_PYTHON_IMPORT_FAILED      = 15021;    /* 0x3AAD */    /* python脚本导入出错 */
    static const int MSP_ERROR_MMP_PYTHON_BAD_FUNC           = 15022;    /* 0x3AAE */    /* python脚本函数格式错误 */
    static const int MSP_ERROR_MMP_DB_DATA_ILLEGAL           = 15023;    /* 0x3AAF */    /* 插入数据库中的数据格式有误 */
    static const int MSP_ERROR_MMP_REDIS_NOT_CONN            = 15024;    /* 0x3AB0 */    /* redis没有连接到服务端 */
    static const int MSP_ERROR_MMP_PMA_NOT_FOUND_STRATEGY    = 15025;    /* 0x3AB1 */    /* 没有找到报警策略 */
    static const int MSP_ERROR_MMP_TAIR_CONNECT              = 15026;    /* 0x3AB2 */    /* 连接tair集群失败 */
    static const int MSP_ERROR_MMP_PMC_SERVINFO_INVALID      = 15027;    /* Ox3AB3 */    /* 此pmc的服务器信息已经无效 */
    static const int MSP_ERROR_MMP_ALARM_GROUP_NULL          = 15028;    /* Ox3AB4 */    /* 服务器报警的短信报警组与邮件报警组均为空 */
    static const int MSP_ERROR_MMP_ALARM_CONTXT_NULL         = 15029;    /* Ox3AB5 */    /* 服务器报警的报警内容为空 */

    /* Error codes of MSC(lmod loader) */
    static const int MSP_ERROR_LMOD_BASE                     = 16000;    /* 0x3E80 */
    static const int MSP_ERROR_LMOD_NOT_FOUND                = 16001;    /* 0x3E81 */    /* 没找到lmod文件 */
    static const int MSP_ERROR_LMOD_UNEXPECTED_BIN           = 16002;    /* 0x3E82 */    /* 无效的lmod */
    static const int MSP_ERROR_LMOD_LOADCODE                 = 16003;    /* 0x3E83 */    /* 加载lmod指令失败 */
    static const int MSP_ERROR_LMOD_PRECALL                  = 16004;    /* 0x3E84 */    /* 初始化lmod失败 */
    static const int MSP_ERROR_LMOD_RUNTIME_EXCEPTION        = 16005;    /* 0x3E85 */    /* lmod运行时异常 */
    static const int MSP_ERROR_LMOD_ALREADY_LOADED           = 16006;    /* 0x3E86 */    /* lmod重复加载 */

    // Error code of Third Business
    static const int MSP_ERROR_BIZ_BASE                      = 17000;    /* 0x4268 */    /* lmod重复加载 */
    static const int MSP_ERROR_NGX_LOG_MORE_TOTEL_SIZE       = 18000; /* Error of Nginx errlog file increase exception */

    // Error Code Of Speech plus
#undef ERROR_NO_NETWORK
    static const int ERROR_NO_NETWORK                                = 20001; /* 无有效的网络连接*/
    static const int ERROR_NETWORK_TIMEOUT                           = 20002; /* 网络连接超时*/
    static const int ERROR_NET_EXCEPTION                             = 20003; /* 网络异常*/
    static const int ERROR_INVALID_RESULT                            = 20004; /* 无有效的结果*/
#undef ERROR_NO_MATCH
    static const int ERROR_AIUI_NO_MATCH                             = 20005; /* 无匹配结果 */
    static const int ERROR_AUDIO_RECORD                              = 20006; /* 录音失败 */
    static const int ERROR_NO_SPEECH                                 = 20007; /* 未检测到语音*/

    static const int ERROR_SPEECH_TIMEOUT                            = 20008; /* 音频输入超时*/
    static const int ERROR_EMPTY_UTTERANCE                           = 20009; /* 无效的文本输入 */
    static const int ERROR_FILE_ACCESS                               = 20010; /* 文件读写失败 */
    static const int ERROR_PLAY_MEDIA                                = 20011; /* 音频播放失败 */

    static const int ERROR_INVALID_PARAM                             = 20012; /* 无效的参数*/
    static const int ERROR_TEXT_OVERFLOW                             = 20013; /* 文本溢出 */
#undef ERROR_INVALID_DATA
    static const int ERROR_INVALID_DATA                              = 20014; /* 无效数据 */
    static const int ERROR_LOGIN                                     = 20015; /* 用户未登陆*/
    static const int ERROR_PERMISSION_DENIED                         = 20016; /* 无效授权 */
    static const int ERROR_INTERRUPT                                 = 20017; /* 被异常打断 */
    static const int ERROR_VERSION_LOWER                             = 20018; /* 版本过低 */
    static const int ERROR_SYSTEM_PREINSTALL                         = 20020; /*系统预置版本 */
    static const int ERROR_UNSATISFIED_LINK                          = 20021; /*未实现的Native函数引用*/
    static const int ERROR_UNKNOWN                                   = 20999; /* 未知错误 */

    static const int ERROR_COMPONENT_NOT_INSTALLED                   = 21001; /* 没有安装语音组件 */
    static const int ERROR_ENGINE_NOT_SUPPORTED                      = 21002; /* 引擎不支持 */
    static const int ERROR_ENGINE_INIT_FAIL                          = 21003; /* 初始化失败 */
    static const int ERROR_ENGINE_CALL_FAIL                          = 21004; /* 调用失败 */
    static const int ERROR_ENGINE_BUSY                               = 21005; /* 引擎繁忙 */

    static const int ERROR_SERVICE_BINDER_DIED                       = 21020; /* 服务组件binder已经消亡 */
    static const int ERROR_IO_EXCEPTION                              = 21021; /* IO异常 */
    static const int ERROR_NOT_WORKING                               = 21022; /* 未处于工作状态 */

    static const int ERROR_LOCAL_NO_INIT                             = 22001; /* 本地引擎未初始化 */
    static const int ERROR_LOCAL_RESOURCE                            = 22002; /* 本地引擎无资源 */
    static const int ERROR_LOCAL_ENGINE                              = 22003; /* 本地引擎内部错误 */
    static const int ERROR_IVW_INTERRUPT                             = 22004; /* 本地唤醒引擎被异常打断 */

    /*Error Code Of Local iflytek Engines*/
    /*Error Code Of AiTalk*/
    /*Error Code Of AiTalk Operation*/
    static const int ERROR_ASR_CLIENT                                = 23000; /* 客户端应用程序错误 */
    static const int ERROR_ASR_INVALID_PARA                          = 23001; /* 无效的参数 */
    static const int ERROR_ASR_INVALID_PARA_VALUE                    = 23002; /* 无效的参数值 */
    static const int ERROR_ASR_OUT_OF_MEMORY                         = 23003; /* 内存耗尽 */
    static const int ERROR_ASR_CREATE_HANDLE_FAILED                  = 23004; /* 创建句柄失败 */
    static const int ERROR_ASR_ENGINE_INIT_FAILED                    = 23005; /* 引擎初始化失败 */
    static const int ERROR_ASR_ENGINE_STARTED                        = 23006; /* 引擎已经启动 */
    static const int ERROR_ASR_ENGINE_UNINIT                         = 23007; /* 引擎未初始化 */
    static const int ERROR_ASR_SPEECH_TIMEOUT                        = 23008; /* 识别超时（VAD没开启或没有检测到后端点） */
    static const int ERROR_ASR_NO_RECOGNIZED_RESULT                  = 23009; /* 无识别结果 */
    static const int ERROR_ASR_INVALID_HANDLE                        = 23010; /* 无效的句柄 */
    static const int ERROR_ASR_FILE_ACCESS                           = 23011; /* 打开文件失败 */

    /*Error Code Of AiTalk Engine*/
    static const int ERROR_AITALK_FALSE                              = 23100; // ivErr_FALSE               = 1
    /* For license check */
    static const int ERROR_AITALK_PERMISSION_DENIED                  = 23101; // ivErr_InvSN               = 2
    /* General */
    static const int ERROR_AITALK_INVALID_PARA                       = 23102; // ivErr_InvArg              = 3
    static const int ERROR_AITALK_BUFFER_OVERFLOW                    = 23103; // ivErr_BufferFull          = 4  /*音频数据缓冲区已满*/
    static const int ERROR_AITALK_FAILED                             = 23104; // ivErr_Failed              = 5
    static const int ERROR_AITALK_NOT_SUPPORTED                      = 23105; // ivErr_NotSupport          = 6  /*引擎不支持*/
    static const int ERROR_AITALK_OUT_OF_MEMORY                      = 23106; // ivErr_OutOfMemory         = 7
    static const int ERROR_AITALK_INVALID_RESOURCE                   = 23107; // ivErr_InvResource         = 8  /*资源无效*/
    static const int ERROR_AITALK_NOT_FOUND                          = 23108; // ivErr_NotFound            = 9  /*打开文件失败*/
    static const int ERROR_AITALK_INVALID_GRAMMAR                    = 23109; // ivErr_InvGrmr             = 10 /*识别语法错误*/
    /* For object status */
    static const int ERROR_AITALK_INVALID_CALL                       = 23110; // ivErr_InvCall             = 11 /*无效调用*/
    /* For ASR Input */
    static const int ERROR_AITALK_SYNTAX_ERROR                       = 23111; // ivErr_InvCall             = 12
    /* For Message Call Back */
    static const int ERROR_AITALK_RESET                              = 23112; // ivErr_Reset               = 13
    static const int ERROR_AITALK_ENDED                              = 23113; // ivErr_Ended               = 14
    static const int ERROR_AITALK_IDLE                               = 23114; // ivErr_Idle                = 15
    static const int ERROR_AITALK_CANNOT_SAVE_FILE                   = 23115; // ivErr_CanNotSaveFile      = 16
    /* For Lexicon name */
    static const int ERROR_AITALK_INVALID_GRAMMAR_NAME               = 23116; // ivErr_InvName             = 17 /*文法或词典名称非法*/
    static const int ERROR_AITALK_BUFFER_EMPTY                       = 23117; // ivErr_BufferEmpty         = 18
    static const int ERROR_AITALK_GET_RESULT                         = 23118; // ivErr_GetResult           = 19
    static const int ERROR_AITALK_REACT_OUT_TIME                     = 23119; // ivErr_ReactOutTime        = 20 /*反应超时*/
    static const int ERROR_AITALK_SPEECH_OUT_TIME                    = 23120; // ivErr_SpeechOutTime       = 21 /*语音超时*/
    static const int ERROR_AITALK_AUDIO_CUT                          = 23121; // ivErr_CUT                 = 22 /*录音质量过高*/
    static const int ERROR_AITALK_AUDIO_LOWER                        = 23122; // ivErr_LOWER               = 23 /*录音质量过低*/
    static const int ERROR_AITALK_INSUFFICIENT_PERMISSIONS           = 23123; // ivErr_Limitted            = 24 /*授权不够*/
    static const int ERROR_AITALK_RESULT_ERROR                       = 23124; // ivErr_ResultError         = 25 /*解码器Wfst输出后，依然有cmd输出*/
    static const int ERROR_AITALK_SHORT_PAUSE                        = 23125; // ivErr_ShortPause          = 26
    static const int ERROR_AITALK_BUSY                               = 23126; // ivErr_Busy                = 27
    static const int ERROR_AITALK_GRM_NOT_UPDATE                     = 23127; // ivErr_GrmNotUpdate        = 28 /*语法未更新*/
    static const int ERROR_AITALK_STARTED                            = 23128; // ivErr_Started             = 29
    static const int ERROR_AITALK_STOPPED                            = 23129; // ivErr_Stopped             = 30
    static const int ERROR_AITALK_ALREADY_STARTED                    = 23130; // ivErr_AlreadyStarted      = 31
    static const int ERROR_AITALK_ALREADY_STOPPED                    = 23131; // ivErr_AlreadyStopped      = 32
    static const int ERROR_AITALK_TOO_MANY_COMMAND                   = 23132; // ivErr_TooManyCmd          = 33
    static const int ERROR_AITALK_WAIT                               = 23133; // ivErr_Wait                = 34 /*程序可能在做一些操作，主线程需要等待*/
    static const int ERROR_AITALK_MAE_RIGHT                          = 23134; // ivErr_MAERight            = 35
    static const int ERROR_AITALK_MAE_WRONG                          = 23135; // ivErr_MAEWrong            = 36
    static const int ERROR_AITALK_GRM_ERR                            = 23300;  // 语法错误

    /*Error Code Of AiSound*/

    /*Error Code Of AiSound Operation*/
    static const int ERROR_TTS_INVALID_PARA                          = 24000; /* 错误参数 */
    static const int ERROR_TTS_INVALID_PARA_VALUE                    = 24001; /* 无效的参数值*/
    static const int ERROR_TTS_OUT_OF_MEMORY                         = 24002; /* 内存不足*/
    static const int ERROR_TTS_INVALID_HANDLE                        = 24003; /* 无效的句柄*/
    static const int ERROR_TTS_CREATE_HANDLE_FAILED                  = 24004; /* 创建句柄失败*/
    static const int ERROR_TTS_INVALID_RESOURCE                      = 24005; /* 无效资源 */
    static const int ERROR_TTS_INVALID_VOICE_NAME                    = 24006; /* 无效发言人*/
    static const int ERROR_TTS_ENGINE_UNINIT                         = 24007; /* 引擎未初始化 */
    static const int ERROR_TTS_ENGINE_INIT_FAILED                    = 24008; /* 引擎初始化失败 */
    static const int ERROR_TTS_ENGINE_BUSY                           = 24009; /* 引擎忙 */


    /*Error Code Of AiSound Engine*/
    static const int ERROR_AISOUND_BASE                               = 24100;
    static const int ERROR_AISOUND_UNIMPEMENTED                       = 24100;  /* unimplemented function */
    static const int ERROR_AISOUND_UNSUPPORTED                        = 24101;  /* unsupported on this platform */
    static const int ERROR_AISOUND_INVALID_HANDLE                     = 24102;  /* invalid handle */
    static const int ERROR_AISOUND_INVALID_PARA                       = 24103;  /* invalid parameter(s) */
    static const int ERROR_AISOUND_INSUFFICIENT_HEAP                  = 24104;  /* insufficient heap size  */
    static const int ERROR_AISOUND_STATE_REFUSE                       = 24105;  /* refuse to do in current state  */
    static const int ERROR_AISOUND_INVALID_PARA_ID                    = 24106;  /* invalid parameter ID */
    static const int ERROR_AISOUND_INVALID_PARA_VALUE                 = 24107;  /* invalid parameter value */
    static const int ERROR_AISOUND_RESOURCE                           = 24108;  /* Resource is error */
    static const int ERROR_AISOUND_RESOURCE_READ                      = 24109;  /* read resource error */
    static const int ERROR_AISOUND_LBENDIAN                           = 24110;  /* the Endian of SDK  is error */
    static const int ERROR_AISOUND_HEADFILE                           = 24111;  /* the HeadFile is different of the SDK */
    static const int ERROR_AISOUND_BUFFER_OVERFLOW                    = 24112;  /* get data size exceed the data buffer */
    static const int ERROR_AISOUND_INVALID_ISAMPA                     = 24113;  /* !Invalid iSampa format or input iSampa text contain invalid alphabet*/
    static const int ERROR_AISOUND_INVALID_CSSML                      = 24114;  /* !Invalid cssml format */

    /*Error Code Of ivw*/

    /*Error Code Of ivw Operation*/
    static const int ERROR_IVW_ENGINE_UNINI             = 25000; /* 引擎未初始化 */
    static const int ERROR_IVW_RESVER_NOMATCH           = 25001; /* 资源版本不匹配 */
    static const int SPEECH_ERROR_IVW_INVALID_RESTYPE   = 25002; /* 不合法的资源类型 */

    /*Error Code Of ivw Engine*/
    static const int ERROR_IVW_INVALID_CALL             = 25101;  // IvwErr_InvCal       = 1
    static const int ERROR_IVW_INVALID_ARG              = 25102;  // IvwErr_InvArg        = 2
    static const int ERROR_IVW_TELL_SIZE                = 25103;  // IvwErr_TellSize     = 3
    static const int ERROR_IVW_OUT_OF_MEMORY            = 25104;  // IvwErr_OutOfMemory  = 4
    static const int ERROR_IVW_OUT_BUFFER_FULL          = 25105;  // IvwErr_BufferFull    = 5
    static const int ERROR_IVW_OUT_BUFFER_EMPTY         = 25106;  // IvwErr_BufferEmpty    = 6
    static const int ERROR_IVW_INVALID_RESOURCE         = 25107;  // IvwErr_InvRes        = 7
    static const int ERROR_IVW_REPETITIOPN_ENTER        = 25108;  // IvwErr_ReEnter        = 8
    static const int ERROR_IVW_NOT_SUPPORT              = 25109;  // IvwErr_NotSupport    = 9
    static const int ERROR_IVW_NOT_FOUND                = 25110;  // IvwErr_NotFound        = 10
    static const int ERROR_IVW_INVALID_SN               = 25111;  // IvwErr_InvSN        = 11
    static const int ERROR_IVW_LIMITTED                 = 25112;  // IvwErr_Limitted        = 12
    static const int ERROR_IVW_TIME_OUT                 = 25113;  // IvwErr_TimeOut        = 13

    static const int ERROR_IVW_ENROLL1_SUCESS           = 25114;  // IvwErr_Enroll1_Success = 14
    static const int ERROR_IVW_ENROLL1_FAILED           = 25115;  // IvwErr_Enroll1_Failed  = 15
    static const int ERROR_IVW_ENROLL2_SUCESS           = 25116;  // IvwErr_Enroll2_Success = 16
    static const int ERROR_IVW_ENROLL2_FAILED           = 25117;  // IvwErr_Enroll2_Failed  = 17
    static const int ERROR_IVW_ENROLL3_SUCESS           = 25118;  // IvwErr_Enroll3_Success = 18
    static const int ERROR_IVW_ENROLL3_FAILED           = 25119;  // IvwErr_Enroll3_Failed  = 19
    static const int ERROR_IVW_SPEECH_TOO_SHORT         = 25120;  // IvwErr_SpeechTooShort  = 20
    static const int ERROR_IVW_SPEECH_STOP              = 25121;  // IvwErr_SpeechStop      = 21

    static const int ERROR_ABILITY_AUTH_DENY       = 25201; /** 离线能力授权拒绝 */
    static const int ERROR_ABILITY_AUTH_NOT_CHECK  = 25202; /** 离线能力授权未检查 */
    static const int ERROR_ABILITY_AUTH_INV_ARG    = 25203; /** 离线能力授权无效参数 */
    static const int ERROR_ABILITY_AUTH_BUSY       = 25204; /** 离线能力授权繁忙 */
    static const int ERROR_ABILITY_AUTH_ERR        = 25205; /** 离线能力授权错误 */

    static const int ERROR_CLOUD_PARAMS_INVALID    = 90003;
};

}

/* clang-format on */
#endif /* AIUIERRORCODE_H_ */

#include <stdio.h>
#include "aiui/aiui_cJSON.h"

typedef AIUI_cJSON cJSON;

static void onmessage(const char *data, int len)
{
    cJSON *result = AIUI_cJSON_Parse(data);

    if (AIUI_cJSON_IsInvalid(result)) {
        return;
    }

    cJSON *action = AIUI_cJSON_GetObjectItem(result, "action");

    if (strcmp(action->valuestring, "result") == 0) {
        cJSON *data = AIUI_cJSON_GetObjectItem(result, "data");

        if (AIUI_cJSON_IsObject(data)) {
            cJSON *sub = AIUI_cJSON_GetObjectItem(data, "sub");

            if (strcmp(sub->valuestring, "iat") == 0) {
                cJSON *text = AIUI_cJSON_GetObjectItem(data, "text");

                if (strlen(text->valuestring) > 0) {
                    printf("user: %s\n", text->valuestring);
                }
            } else if (strcmp(sub->valuestring, "nlp") == 0) {
                cJSON *intent = AIUI_cJSON_GetObjectItem(data, "intent");
                if (AIUI_cJSON_IsObject(intent)) {
                    cJSON *rc = AIUI_cJSON_GetObjectItem(intent, "rc");

                    if (AIUI_cJSON_IsNumber(rc)) {
                        if (rc->valueint == 0) {
                            cJSON *answer = AIUI_cJSON_GetObjectItem(intent, "answer");
                            cJSON *text = AIUI_cJSON_GetObjectItem(answer, "text");

                            printf("server: %s\n", text->valuestring);
                        }
                    }
                }

            } else if (strcmp(sub->valuestring, "tts") == 0) {
                cJSON *content = AIUI_cJSON_GetObjectItem(data, "content");
                int decLen = strlen(content->valuestring) / 4.0 * 3 + 50;

                char *output = (char *)malloc(decLen);
                if ((decLen = aiui_base64_decode(content->valuestring, output, decLen)) != -1) {
                    printf("tts -> decLen: %d\n", decLen);
                }
            }
        }
    } else if (strcmp(action->valuestring, "vad") == 0) {
        printf("aiui %s\n", data);
        aiui_finished();
    } else if (strcmp(action->valuestring, "started") == 0) {
        printf("aiui login success %s\n", data);
    } else if (strcmp(action->valuestring, "error") == 0) {
        printf("aiui %s\n", data);
    } else {
        printf("aiui %s\n", data);
    }

    AIUI_cJSON_Delete(result);
}

static void onerror(int code, const char *str)
{
    printf("aiui_init onerror %d => %s\n", code, str);
}

typedef void (*aiui_message_fn)(const char *data, int len);
typedef void (*aiui_error_fn)(int code, const char *str);

int aiui_init(const char *appid,
              const char *key,
              const char *param,
              aiui_message_fn m_cb,
              aiui_error_fn e_cb);

int aiui_write(const char *audio, int len);
int aiui_finished();
void aiui_destroy();

int main()
{
    const char *appid = "XXXXXXX"; // 应用ID，在AIUI开放平台创建并设置
    const char *key = "XXXXXXXXXXXXXXXXXXXXXXX"; // 接口密钥，在AIUI开放平台查看

#if 1
    const char *param =
        "{"
        "\"auth_id\":\"ac30105366ea460f9ff08ddac0c4f71e\"," // auth_id 与设备有关，每个设备不一样，自己生成
        "\"result_level\":\"plain\","
        "\"data_type\":\"audio\","
        "\"aue\":\"raw\","
        "\"scene\":\"main_box\","
        "\"sample_rate\":\"16000\", "
        "\"vad_info\":\"end\","
        "\"cloud_vad_eos\":\"700\","
        "\"close_delay\":\"200\","
        "\"context\":\"{\\\"sdk_support\\\":[\\\"iat\\\",\\\"nlp\\\",\\\"tts\\\"]}\"}";

    aiui_init(appid, key, param, onmessage, onerror);

    // 发送的数据文件位置
    FILE *fp = fopen("test.pcm", "rb");

    if(!fp) {
        printf("cant not find test.pcm !!!\n");
        aiui_finished();
        aiui_destroy();
        exit(-1);
    }

    char buff[1280];
    memset(buff, '\0', 1280);

    while (fread(buff, sizeof(char), 1280, fp) > 0) {
        if (aiui_write(buff, 1280) != 0) {
            break;
        }

        usleep(40 * 1000);

        memset(buff, '\0', 1280);
    }
#else
    const char *param =
            "{\"result_level\":\"plain\",\"auth_id\":\"ac30105366ea460f9ff08ddac0c4f71e\",\"data_"
            "type\":\"text\","
            "\"scene\":\"main_box\",\"sample_rate\":\"16000\", "
            "\"context\":\"{\\\"sdk_support\\\":[\\\"nlp\\\",\\\"tts\\\"]}\"}";

    aiui_init(appid, key, param, onmessage, onerror);

    const char *text= "合肥明天的天气怎么样？";

    aiui_write(text, strlen(text));

#endif

    aiui_finished();

    aiui_destroy();
}
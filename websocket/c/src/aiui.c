#include "time.h"

#include "aiui/aiui_base64.h"
#include "aiui/aiui_sha256.h"
#include "pthread.h"

#define aiui_printf(...) 

typedef void (*aiui_message_fn)(const char *data, int len);
typedef void (*aiui_error_fn)(int code, const char *str);

static struct wsclient *g_ws_client = NULL;
static aiui_message_fn g_message_cb = NULL;
static aiui_error_fn g_error_cb = NULL;

static int g_isopened = 0;

static const char *END_FLAG = "--end--";

typedef struct _wsclient_message
{
    unsigned int opcode;
    unsigned long long payload_len;
    char *payload;
} wsclient_message;

typedef struct _wsclient_error
{
    int code;
    int extra_code;
    char *str;
} wsclient_error;

struct wsclient;

extern struct wsclient *libwsclient_new(char *URI);

extern void libwsclient_run(struct wsclient *c);

extern void libwsclient_finish(struct wsclient *client);

extern void libwsclient_close(struct wsclient *c);

extern int libwsclient_send_fragment(struct wsclient *client, char *strdata, int len, int flags);

extern int libwsclient_send(struct wsclient *client, char *strdata);

extern void libwsclient_onclose(struct wsclient *client, int (*cb)(struct wsclient *c));

extern void libwsclient_onopen(struct wsclient *client, int (*cb)(struct wsclient *c));

extern void libwsclient_onmessage(struct wsclient *client,
                                  int (*cb)(struct wsclient *c, wsclient_message *msg));

extern void libwsclient_onerror(struct wsclient *client,
                                int (*cb)(struct wsclient *c, wsclient_error *err));

static int aiui_onerror(struct wsclient *c, wsclient_error *err)
{
    if (g_error_cb) {
        printf("aiui_init aiui_onerror\n");
        g_error_cb(err->code, err->str);
    }

    return 0;
}

static int aiui_onmessage(struct wsclient *c, wsclient_message *msg)
{
    if (g_message_cb) {
        aiui_printf("aiui_init onmessage\n");

        g_message_cb(msg->payload, msg->payload_len);
    }

    return 0;
}

static int aiui_onopen(struct wsclient *c)
{
    aiui_printf("aiui_init onopen\n");

    g_isopened = 1;

    return 0;
}

static int aiui_onclose(struct wsclient *c)
{
    aiui_printf("aiui_init onclose\n");

    g_ws_client = NULL;
    g_message_cb = NULL;
    g_error_cb = NULL;
    g_isopened = 0;

    return 0;
}

extern int aiui_base64_encode(unsigned char *source, size_t sourcelen, char *target, size_t targetlen);

extern int mbedtls_sha256_ret(const unsigned char *input,
                              size_t ilen,
                              unsigned char output[32],
                              int is224);

int aiui_vaild()
{
    return (g_ws_client != NULL) && (g_isopened == 1);
}

int aiui_init(
    const char *appid, const char *key, const char *param, aiui_message_fn m_cb, aiui_error_fn e_cb)
{
    g_message_cb = m_cb;
    g_error_cb = e_cb;

    if (aiui_vaild()) {
        return 0;
    }

    aiui_printf("aiui_init init start: 1\n");

    char time_str[20];
    memset(time_str, '\0', 20);
    snprintf(time_str, 19, "%d", time(NULL));

    int base_str_len = (strlen(param) + 2) / 3 * 4 + 4;

    char *encodeStr = (char *)malloc(base_str_len);
    if (!encodeStr) {
        printf("aiui_init malloc encodeStr error\n");
        return -1;
    }

    memset(encodeStr, '\0', base_str_len);
    if (aiui_base64_encode((unsigned char *)param, strlen(param), encodeStr, base_str_len) == 0) {
        aiui_printf("aiui_init buildUrl 5\n");
        free(encodeStr);
        encodeStr = NULL;
        return -1;
    }

    aiui_printf("aiui_init buildUrl 3 \n");

    base_str_len = strlen(key) + strlen(time_str) + strlen(encodeStr) + 4;

    char *originStr = (char *)malloc(base_str_len);
    if (!originStr) {
        aiui_printf("aiui_init malloc originStr error\n");
        return -1;
    }

    memset(originStr, '\0', base_str_len);
    snprintf(originStr, base_str_len - 1, "%s%s%s", key, time_str, encodeStr);

    char hex_output[80];
    {
        unsigned char output[32];

        memset(output, '\0', 32);
        memset(hex_output, '\0', 80);

        mbedtls_sha256_ret((const unsigned char *)originStr, strlen(originStr), output, 0);

        int i = 0;
        for (; i < 32; ++i) {
            sprintf(hex_output + i * 2, "%02x", output[i]);
        }

        aiui_printf("aiui_init buildUrl 5 \n");
    }

    base_str_len += 256;

    char *url = (char *)malloc(base_str_len);
    if (!url) {
        return -1;
    }

    memset(url, '\0', base_str_len);
    snprintf(url,
             base_str_len - 1,
             "/v1/aiui?appid=%s&checksum=%s&curtime=%s&param=%s&signtype=sha256",
             appid,
             hex_output,
             time_str,
             encodeStr);

    free(encodeStr);
    encodeStr = NULL;

    free(originStr);
    originStr = NULL;

    g_ws_client = libwsclient_new(url);

    aiui_printf("aiui_init buildUrl 8 \n");

    if (!g_ws_client) {
        printf("aiui_init libwsclient_new error\n");
        return -1;
    }

    aiui_printf("aiui_init buildUrl 9 \n");

    libwsclient_onopen(g_ws_client, aiui_onopen);
    libwsclient_onmessage(g_ws_client, aiui_onmessage);
    libwsclient_onerror(g_ws_client, aiui_onerror);
    libwsclient_onclose(g_ws_client, aiui_onclose);

    aiui_printf("aiui_init buildUrl 10 \n");

    libwsclient_run(g_ws_client);

    aiui_printf("aiui_init buildUrl 11 \n");

    return 0;
}

int aiui_write(const char *audio, int len)
{
    int ret = 0;
    do {
        if (!aiui_vaild()) {
            ret = -1;
            break;
        }

        ret = libwsclient_send_fragment(g_ws_client, audio, len, 0x20);
    } while (0);

    return ret == 0;
}

int aiui_finished()
{
    int ret = 0;
    do {
        if (!aiui_vaild()) {
            ret = 1;
            break;
        }

        ret = libwsclient_send_fragment(g_ws_client, END_FLAG, strlen(END_FLAG), 0x21);
    } while (0);

    return ret;
}

void aiui_destroy()
{
    do {
        if (!aiui_vaild()) {
            break;
        }

        libwsclient_finish(g_ws_client);

        g_ws_client = NULL;
        g_message_cb = NULL;
        g_error_cb = NULL;
    } while (0);
}

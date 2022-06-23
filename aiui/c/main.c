#include "aiui/AIUI_C.h"
#include "aiui/AIUIConstant.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef WIN32
#    include <windows.h>
#    define AIUI_SLEEP Sleep
#else
#    include <unistd.h>
#    define AIUI_SLEEP(x) usleep(x * 1000)
#endif


void event(const AIUIEvent ae, void *data)
{
    printf("type: %d, info: %s\n", aiui_event_type(ae), aiui_event_info(ae));

     AIUIDataBundle db = aiui_event_databundle(ae);
     if(db) {
        int len = 0;
        const char *res = aiui_db_binary(db, "0", &len);
        if(len != 0) {
            char *result = (char *)malloc(len + 1);
            memset(result, '\0', (len + 1));
            memcpy(result, res, len);

            printf("result %s\n", result);

            free(result);
        }
     }
}

int get_file_size(const char* file)
{
	int size = 0;
#ifdef _WIN64
	struct __stat64 si;
#endif
	if ( file != 0 )
	{
#ifdef _WIN64
		int ret = ::_stat64(file, &si);
		if ( ret == 0 )
			size = si.st_size;
#else
		FILE * pTest = fopen(file,"r");
		if(pTest != NULL)
		{
			fseek(pTest, 0, SEEK_END);
			size = ftell(pTest);
			fclose(pTest);
		}
		else
		{	
			size = 0;
		}
#endif
		
	}
	return size;
}

int read_bin_file (const char * file, void * data, size_t bytes, int * readed /* = 0 */)
{
	FILE * fp = fopen(file, "rb");
	if ( fp != 0 )
	{
		int rs = fread(data, 1, bytes, fp);
		if ( readed )
			*readed = rs;
		fclose(fp);
		return 0;
	}

	return -1;
}


int read_file_mem (const char *pResFilePath, char **pResBuf, int *pResSize)
{
    if (pResFilePath == NULL || pResBuf == NULL || pResSize == NULL) {
        return -1;
    }

    size_t resSize = get_file_size(pResFilePath);
    if (resSize == 0) {
        return -2;
    }

    char *pRes = (char *)malloc(resSize);
    int ret = read_bin_file(pResFilePath, pRes, resSize, NULL);

    if (ret != 0) {
        free(pRes);
        *pResBuf = NULL;
        *pResSize = 0;
        return -3;
    }

    *pResBuf = pRes;
    *pResSize = resSize;

    return 0;
}


#if defined(__linux__)
#    include <sys/socket.h>
#    include <net/if.h>
#    include <sys/ioctl.h>

static void GenerateMACAddress(char* mac)
{
    /* reference: https://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program/35242525 */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return;
    };

    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        return;
    }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    struct ifreq ifr;

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) {    /* don't count loopback */
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        } else {
            return;
        }
    }

    unsigned char mac_address[6];
    if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);

    sprintf(mac,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_address[0],
            mac_address[1],
            mac_address[2],
            mac_address[3],
            mac_address[4],
            mac_address[5]);
}
#elif defined(_WIN32)
#    include <stdio.h>
#    include <IPHlpApi.h>
#    pragma comment(lib, "IPHLPAPI.lib")

static void GenerateMACAddress(char* mac)
{
    ULONG ulSize = 0;
    PIP_ADAPTER_INFO adapterInfo = NULL;
    PIP_ADAPTER_INFO adapterInfoTmp = NULL;

    GetAdaptersInfo(adapterInfo, &ulSize);

    if (0 == ulSize) {
        return;
    }

    adapterInfo = (PIP_ADAPTER_INFO)malloc(ulSize);

    if (adapterInfo == NULL) {
        return;
    }

    memset(adapterInfo, 0, ulSize);

    adapterInfoTmp = adapterInfo;

    GetAdaptersInfo(adapterInfo, &ulSize);

    while (adapterInfo) {
        _snprintf(mac,
                  64,
                  "%02x:%02x:%02x:%02x:%02x:%02x",
                  adapterInfo->Address[0],
                  adapterInfo->Address[1],
                  adapterInfo->Address[2],
                  adapterInfo->Address[3],
                  adapterInfo->Address[4],
                  adapterInfo->Address[5]);

        if (strcmp(adapterInfo->GatewayList.IpAddress.String, "0.0.0.0") != 0) break;

        adapterInfo = adapterInfo->Next;
    }

    free(adapterInfoTmp);

    adapterInfoTmp = NULL;
}
#endif

int main()
{
    char *param;
    int size;

    read_file_mem("./AIUI/cfg/aiui.cfg", &param, &size);

    char mac[64] = {0};
    GenerateMACAddress(mac);    /* 设备唯一ID, 可自己重新实现 */

    /*  
        为每一个设备设置对应唯一的SN
        最好使用设备硬件信息(mac地址，设备序列号等）生成），
        以便正确统计装机量，避免刷机或者应用卸载重装导致装机量重复计数 
    */
    aiui_set_system_info(AIUI_KEY_SERIAL_NUM, mac);

    AIUIAgent agent = aiui_agent_create(param, event, NULL);

    free(param);

    if(NULL == agent) {
        printf("agent create error");
        return -1;
    }

    AIUI_SLEEP(3);

    AIUIMessage wakeupMsg = aiui_msg_create1(AIUI_CMD_WAKEUP);
    aiui_agent_send_message(agent, wakeupMsg);
    aiui_msg_destroy(wakeupMsg);


    const char * text = "合肥明天天气怎么样";

    AIUI_SLEEP(3);

    /* textData内存会在Message在内部处理完后自动release掉 */
    AIUIBuffer textData = aiui_create_buffer_from_data(text, strlen(text));

    AIUIMessage writeMsg = aiui_msg_create(2, 0, 0, "data_type=text,tts_res_type=url", textData);
    aiui_agent_send_message(agent, writeMsg);
    aiui_msg_destroy(writeMsg);

    while(1){};
}

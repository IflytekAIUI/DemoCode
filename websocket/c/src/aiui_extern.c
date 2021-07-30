#include <pthread.h>

 aiui_init_pthread_attr(pthread_attr_t *attr, int size)
{
    pthread_attr_init(attr);
    pthread_attr_setstacksize(attr, size);
}

void aiui_destroy_pthread_attr(pthread_attr_t *attr)
{
    pthread_attr_destroy(attr);
}
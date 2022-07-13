/*
 * AIUIType.h
 *
 *  Created on: 2017年2月17日
 *      Author: AIUI开放平台（https://aiui.xfyun.cn/）
 */

#ifndef AIUITYPE_H_
#define AIUITYPE_H_

#include "AIUICommon.h"

#include <stdint.h>
#include <sys/types.h>

namespace aiui {

/*
 内存管理类，用于管理一段内存，可当作带长度的数组使用。
 */
class Buffer
{
public:
    enum { eKeepStorage = 0x00000001 };

    /*
     在堆上分配一个Buffer对象，返回其指针。
     @param size Buffer大小
     */
    AIUIEXPORT static Buffer* alloc(size_t size);

    /*
     当引用计数为0时，回收已分配的Buffer内存。
     @param released Buffer指针
     @return 0成功，-1失败
     */
    AIUIEXPORT static ssize_t dealloc(const Buffer* released);

    /*
     获取只可读的Buffer数据地址。
     @return 数据地址
     */
    AIUIEXPORT inline const void* data() const;

    /*
     获取可读写的Buffer中数据地址。
     @return 数据地址
     */
    AIUIEXPORT inline void* data();

    /*
     获取Buffer的大小（字节数）。
     @return Buffer大小
     */
    AIUIEXPORT inline size_t size() const;

    /*
     由数据地址得到Buffer指针（可读写）。
     @param data 数据地址
     @param 对应的Buffer指针
     */
    AIUIEXPORT static inline Buffer* bufferFromData(void* data);

    /*
	 由数据地址得到Buffer指针（只可读）。
	 @param 数据地址
	 @param 对应的Buffer指针
	 */
    AIUIEXPORT static inline const Buffer* bufferFromData(const void* data);

    /*
     获取Buffer大小。
     @param data()方法返回的数据地址
     @return Buffer大小
     */
    AIUIEXPORT static inline size_t sizeFromData(const void* data);

    /*
     获取可读写的Buffer指针。
     @return 可读写的Buffer指针
     */
    AIUIEXPORT Buffer* edit() const;

    /*
     重新设置Buffer大小，并返回Buffer指针，当Buffer变小时会丢失部分原始数据。
     @param size 新Buffer大小
     @param Buffer指针
     */
    AIUIEXPORT Buffer* editResize(size_t newSize) const;

    /*
     当Buffer引用计数为1时，返回Buffer指针，否则返回NULL。
     @return Buffer指针
     */
    AIUIEXPORT Buffer* attemptEdit() const;

    /*
     重新设置Buffer大小，原始数据会丢失。
     @param size Buffer大小
     @return 新Buffer指针
     */
    AIUIEXPORT Buffer* reset(size_t size) const;

    /*
     增加Buffer的引用计数。
     */
    AIUIEXPORT void acquire() const;

    /*
     当Buffer的引用计数为1时，释放Buffer所占内存。
     */
    AIUIEXPORT int32_t release(uint32_t flags = 0) const;

    /*
      获取Buffer的引用计数是否为1。
     */
    AIUIEXPORT inline bool onlyOwner() const;

    /*
     获取Buffer的拷贝，会在堆上重新分配内存。
     @return 拷贝对象指针
     */
    AIUIEXPORT Buffer* copy();

private:
    inline Buffer()
        : mRefs(0)
        , mSize(0)
    {
    }

    inline ~Buffer() {}

    Buffer(const Buffer&);

    Buffer& operator=(const Buffer&);

    mutable int32_t mRefs;

    size_t mSize;

    uint32_t mReserved[2];
};

/*
  数据捆绑类，支持int、long、string和Buffer*类型数据捆绑传输。
 */
class IDataBundle
{
public:
    AIUIEXPORT virtual ~IDataBundle();

    /*
	 创建数据捆绑对象。
	 @return 对象指针
	 */
    AIUIEXPORT static IDataBundle* create();

    /*
	  销毁对象。
	 */
    virtual void destroy() = 0;

    /*
	 移除key对应的项目。
	 @param key 项目的key值
	 */
    virtual bool remove(const char* key) = 0;

    /* 
	 以下方法用于添加（获取）int、string、Buffer*类型数据。
	*/
    virtual bool putInt(const char* key, int val, bool replace = false) = 0;

    virtual int getInt(const char* key, int defaultVal) = 0;

    virtual bool putLong(const char* key, long val, bool replace = false) = 0;

    virtual long getLong(const char* key, long defaultVal) = 0;

    virtual bool putString(const char* key, const char* val, bool replace = false) = 0;

    /**
	  注：getString返回地址对应的内存由IDataBundle管理，不可（不需要）在外部delete或者free。
	 **/
    virtual const char* getString(const char* key, const char* defaultVal) = 0;

    virtual bool putBinary(const char* key,
                           const char* data,
                           int dataLen,
                           bool replace = false) = 0;

    virtual const char* getBinary(const char* key, int* dataLen) = 0;
};

// ---------------------------------------------------------------------------

const void* Buffer::data() const
{
    return this + 1;
}

void* Buffer::data()
{
    return this + 1;
}

size_t Buffer::size() const
{
    return mSize;
}

Buffer* Buffer::bufferFromData(void* data)
{
    return data ? static_cast<Buffer*>(data) - 1 : 0;
}

const Buffer* Buffer::bufferFromData(const void* data)
{
    return data ? static_cast<const Buffer*>(data) - 1 : 0;
}

size_t Buffer::sizeFromData(const void* data)
{
    return data ? bufferFromData(data)->mSize : 0;
}

bool Buffer::onlyOwner() const
{
    return (mRefs == 1);
}

}    // namespace aiui

#ifdef AIUI_LIB_COMPILING
#    include "aiui_internal/AIUIType_internal.h"
#endif

#endif /* AIUITYPE_H_ */

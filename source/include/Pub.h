/*************************************************************************
    > File Name: Pub.h
    > Author: Leosocy
    > Mail: 513887568@qq.com
    > Created Time: 2017/10/1 23:05:54
************************************************************************/
#ifndef __PUB_H__
#define __PUB_H__

#include <string>

#define _IN
#define _OUT
#define _INOUT

typedef unsigned short u_short;

namespace EDCC {
    static const char hexArray[16] =
    {'0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'};

    #define IMAGE_SIZE_W "imageSizeW"
    #define IMAGE_SIZE_H "imageSizeH"
    #define GABOR_KERNEL_SIZE "gaborKernelSize"
    #define GABOR_DIRECTIONS "gaborDirections"
    #define LAPLACE_KERNEL_SIZE "laplaceKernelSize"

    #define CHECK_POINTER_NULL_RETURN(pointer, ret) do { \
        if((pointer) == NULL) { \
            return (ret); \
        } \
    } while(0)

    #define CHECK_POINTER_NULL_RETURN_VOID(pointer) do { \
        if((pointer) == NULL) { \
            return ; \
        } \
    } while(0)

    #define CHECK_NE_RETURN_VOID(param1, param2) do { \
        if((param1) != (param2)) { \
            return ; \
        } \
    } while(0)

    #define CHECK_NE_RETURN(param1, param2, ret) do { \
        if((param1) != (param2)) { \
            return (ret); \
        } \
    } while(0)

    #define CHECK_EQ_RETURN_VOID(param1, param2) do { \
        if((param1) == (param2)) { \
            return ; \
        } \
    } while(0)

    #define CHECK_EQ_RETURN(param1, param2, ret) do { \
        if((param1) == (param2)) { \
            return (ret); \
        } \
    } while(0)

    std::string toUpper(_IN const char *src);

    void EDCC_Log(_IN const char *format, _IN ...);
}

#endif // !__PUB_H__
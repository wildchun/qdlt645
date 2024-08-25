// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 14:14
// Description : ...
//
// **************************************************

#ifndef QDLT645_H
#define QDLT645_H

#include <QtGlobal>

#ifdef QDLT645_LIB
    #define QDLT645_EXPORT Q_DECL_EXPORT
#else
    #define QDLT645_EXPORT Q_DECL_IMPORT
#endif

#define QDLT645_FRAME_PREFIX  quint8(0xFE)
#define QDLT645_FRAME_HEADER  quint8(0x68)
#define QDLT645_WILDCARD_ADDR quint8(0xAA)
#define QDLT645_ADDRESS_LEN   (6)

namespace qdlt645 {

}// namespace qdlt645

#endif//QDLT645_H

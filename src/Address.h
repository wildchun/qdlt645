// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 14:22
// Description : ...
//
// **************************************************

#ifndef ADDRESS_H
#define ADDRESS_H
#include "QDLT645.h"

#include <QByteArray>

namespace qdlt645 {
class Stream;

class QDLT645_EXPORT Address {
    friend Stream &operator<<(Stream &s, const Address &a);
    friend Stream &operator>>(Stream &s, Address &a);

public:
    static const Address BroadcastAddress;
    static bool isAddrString(const QString &addrStr);
    static bool isWildcardAddrString(const QString &addrStr);
    static Address fromAddrString(const QString &addrStr);
public:
    Address();
    explicit Address(const QByteArray &addr);
    void setAddrBytes(const QByteArray &addr);
    void setAddrByte(int index, quint8 value);

    QByteArray getAddrBytes() const;
    quint8 getAddrByte(int index) const;

    bool isWildcardAddr()const;
private:
    quint8 mAddrBytes[QDLT645_ADDRESS_LEN];
};
}// namespace qdlt645

#endif//ADDRESS_H

// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 14:22
// Description : ...
//
// **************************************************

#include "Address.h"

#include "Stream.h"

namespace qdlt645 {
const Address Address::BroadcastAddress = Address(QByteArrayLiteral("\x99\x99\x99\x99\x99\x99"));
#define DEFAULT_ADDRESS \
    mAddrBytes { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

Address Address::fromAddrString(const QString &addrStr) {
    QByteArray addrBytes = QByteArray::fromHex(addrStr.toLatin1());
    return Address(addrBytes);
}

bool Address::isAddrString(const QString &addrStr) {
    if (addrStr.size() > QDLT645_ADDRESS_LEN * 2) {
        return false;
    }
    return std::any_of(addrStr.begin(), addrStr.end(), [](QChar c) {
        return c.isDigit();
    });
}

bool Address::isWildcardAddrString(const QString &addrStr) {
    if (addrStr.size() > QDLT645_ADDRESS_LEN * 2 || addrStr.size() % 2 != 0) {
        return false;
    }
    //地址域支持缩位寻址，即从若干低位起，剩余高位补 AAH 作为通配符进行读表操作，从站应答帧的地址域返回实际通信地址。
    bool findWildcard = false;
    for (int i = 0; i < addrStr.size(); i += 2) {
        if (addrStr.mid(i, 2) == "AA") {
            findWildcard = true;
            continue;
        }
    }
    return true;
}

Address::Address() :
    DEFAULT_ADDRESS {
    memset(mAddrBytes, 0, QDLT645_ADDRESS_LEN);
}

Address::Address(const QByteArray &addr) :
    DEFAULT_ADDRESS {
    setAddrBytes(addr);
}

void Address::setAddrBytes(const QByteArray &addr) {
    if (addr.size() < QDLT645_ADDRESS_LEN) {
        int ofs = QDLT645_ADDRESS_LEN - addr.size();
        memset(mAddrBytes, 0, QDLT645_ADDRESS_LEN);
        memcpy(mAddrBytes + ofs, addr.constData(), addr.size());
    } else {
        memcpy(mAddrBytes, addr.constData(), QDLT645_ADDRESS_LEN);
    }
}

QByteArray Address::getAddrBytes() const {
    return {reinterpret_cast<const char *>(mAddrBytes), QDLT645_ADDRESS_LEN};
}

Stream &operator<<(Stream &s, const Address &a) {
    s << Stream::CArray(reinterpret_cast<const char *>(a.mAddrBytes), QDLT645_ADDRESS_LEN);
    return s;
}

Stream &operator>>(Stream &s, Address &a) {
    s >> Stream::CArray(reinterpret_cast<char *>(a.mAddrBytes), QDLT645_ADDRESS_LEN);
    return s;
}

quint8 Address::getAddrByte(int index) const {
    if (index < 0 || index >= QDLT645_ADDRESS_LEN) {
        return 0;
    }
    return mAddrBytes[index];
}

void Address::setAddrByte(int index, quint8 value) {
    if (index < 0 || index >= QDLT645_ADDRESS_LEN) {
        return;
    }
    mAddrBytes[index] = value;
}

bool Address::isWildcardAddr() const {
}


}// namespace qdlt645
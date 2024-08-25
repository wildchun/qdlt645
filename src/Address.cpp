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
const Address Address::InvalidAddress = Address(QByteArrayLiteral("\x00\x00\x00\x00\x00\x00"));
#define DEFAULT_ADDRESS \
    mAddrBytes { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

Address Address::fromString(const QString &addrStr) {
    QByteArray addrBytes = QByteArray::fromHex(addrStr.toLatin1());
    return Address(addrBytes);
}

bool Address::isAddrBytes(const QByteArray &addrBytes) {
    if (addrBytes.size() > QDLT645_ADDRESS_LEN * 2) {
        return false;
    }
    return std::all_of(addrBytes.begin(), addrBytes.end(), [](char c) {
        quint8 h = static_cast<quint8>(c) >> 4;
        quint8 l = static_cast<quint8>(c) & 0x0F;
        return (h < 10 && l < 10) || (c == char(0xAA));
    });
}

bool Address::isWildcardAddrBytes(const QByteArray &addrBytes) {
    if (addrBytes.size() != QDLT645_ADDRESS_LEN) {
        return false;
    }
    if (!addrBytes.startsWith(char(QDLT645_WILDCARD_ADDR))) {
        return false;
    }
    bool findNotAA = false;
    for (auto b: addrBytes) {
        if (findNotAA) {
            if (b == char(QDLT645_WILDCARD_ADDR)) {
                return false;
            }
        } else {
            if (b != char(QDLT645_WILDCARD_ADDR)) {
                findNotAA = true;
            }
        }
    }
    return true;
}

bool Address::isAddrString(const QString &addrStr) {
    QByteArray addrBytes = QByteArray::fromHex(addrStr.toLatin1());
    if (addrBytes.size() != addrStr.size() / 2) {
        return false;
    }
    return isAddrBytes(addrBytes);
}

bool Address::isWildcardAddrString(const QString &addrStr) {
    QByteArray addrBytes = QByteArray::fromHex(addrStr.toLatin1());
    if (addrBytes.size() != addrStr.size() / 2) {
        return false;
    }
    return isWildcardAddrBytes(addrBytes);
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
    // 地址域传输时低字节在前，高字节在后。
    //mAddrBytes 存储时高字节在前，低字节在后。
    char addrBytes[QDLT645_ADDRESS_LEN];
    for (int i = 0; i < QDLT645_ADDRESS_LEN; ++i) {
        addrBytes[i] = (char) a.mAddrBytes[QDLT645_ADDRESS_LEN - 1 - i];
    }
    s << Stream::CArray(addrBytes, QDLT645_ADDRESS_LEN);
    return s;
}

Stream &operator>>(Stream &s, Address &a) {
    char addrBytes[QDLT645_ADDRESS_LEN];
    s >> Stream::CArray(addrBytes, QDLT645_ADDRESS_LEN);
    for (int i = 0; i < QDLT645_ADDRESS_LEN; ++i) {
        a.mAddrBytes[i] = (quint8) addrBytes[QDLT645_ADDRESS_LEN - 1 - i];
    }
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

bool Address::isWildcard() const {
    return isWildcardAddrBytes(QByteArray{reinterpret_cast<const char *>(mAddrBytes), QDLT645_ADDRESS_LEN});
}

bool Address::isBroadcast() const {
    return *this == BroadcastAddress;
}

bool Address::operator==(const Address &other) const {
    return memcmp(mAddrBytes, other.mAddrBytes, QDLT645_ADDRESS_LEN) == 0;
}

bool Address::operator!=(const Address &other) const {
    return !(*this == other);
}

bool Address::isMatch(const Address &other) const {
    if (!other.isWildcard()) {
        return *this == other;
    }
    for (int idx = 0; idx < QDLT645_ADDRESS_LEN; ++idx) {
        if (other.getAddrByte(idx) != QDLT645_WILDCARD_ADDR
            && getAddrByte(idx) != other.getAddrByte(idx)) {
            return false;
        }
    }
}

Address Address::createWildcard(const QByteArray &lowAddr) {
    QByteArray wcAddr = lowAddr;
    if (wcAddr.size() < QDLT645_ADDRESS_LEN) {
        wcAddr = wcAddr.rightJustified(QDLT645_ADDRESS_LEN, char(0xAA));
        return Address(wcAddr);
    } else if (wcAddr.size() > QDLT645_ADDRESS_LEN) {
        return Address(wcAddr.left(QDLT645_ADDRESS_LEN));
    }
    return Address(wcAddr);
}

}// namespace qdlt645
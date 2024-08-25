// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 15:28
// Description : ...
//
// **************************************************

#include "Address.h"
#include "Stream.h"

#include <QDebug>
using namespace qdlt645;

int main(int argc, char *argv[]) {
    {
        Address a(Address::BroadcastAddress);
        QByteArray ba;
        Stream s(&ba);
        s << a;

        s.seek(0);

        Address b;
        s >> b;

        qDebug() << a.getAddrBytes().toHex(' ').toUpper();
        qDebug() << b.getAddrBytes().toHex(' ').toUpper();
        qDebug() << "a.isBroadcast()" << a.isBroadcast();
        qDebug() << "a.isWildcard()" << a.isWildcard();
    }
    {
        Address a = Address::fromString("010203040506");
        QByteArray ba;
        Stream s(&ba);
        s << a;
        qDebug() << "ba" << ba.toHex(' ').toUpper();

        s.seek(0);

        Address b;
        s >> b;

        qDebug() << a.getAddrBytes().toHex(' ').toUpper();
        qDebug() << b.getAddrBytes().toHex(' ').toUpper();
        qDebug() << "a.isBroadcast()" << a.isBroadcast();
        qDebug() << "a.isWildcard()" << a.isWildcard();
    }
    qDebug() << "-------------------------------------";
    {
        QStringList addrList;
        addrList << "AAAAAAAAAAAA";
        addrList << "AAAAAAAAAA99";
        addrList << "AAAAAAAA0010";
        addrList << "AAA00AAA0010";
        addrList << "000000000000";
        addrList << "000000000001";
        addrList << "0000000001";
        addrList << "00000000001";
        for (const auto &addrStr: addrList) {
            qDebug() << "addrStr" << addrStr << "[" << addrStr.size() << "]";
            qDebug() << "Address::isAddrString" << Address::isAddrString(addrStr);
            qDebug() << "Address::isWildcardAddrString" << Address::isWildcardAddrString(addrStr);
        }
    }
    qDebug() << "-------------------------------------";
    return 0;
}

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
    Address a(Address::BroadcastAddress);
    QByteArray ba;
    Stream s(&ba);
    s << a;

    s.seek(0);

    Address b;
    s >> b;

    qDebug() << a.getAddrBytes().toHex(' ').toUpper();
    qDebug() << b.getAddrBytes().toHex(' ').toUpper();
    return 0;
}

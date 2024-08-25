// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 14:30
// Description : ...
//
// **************************************************

#include "Stream.h"

#include <QBuffer>
#include <QIODevice>
#include <QtEndian>

#define DEFAULT_NO_SWAP (QSysInfo::ByteOrder == QSysInfo::BigEndian)


#undef CHECK_STREAM_PRECOND
#ifndef QT_NO_DEBUG
    #define CHECK_STREAM_PRECOND(retVal)   \
        if (!mDev) {                       \
            qWarning("Stream: No device"); \
            return retVal;                 \
        }
#else
    #define CHECK_STREAM_PRECOND(retVal) \
        if (!mDev) {                     \
            return retVal;               \
        }
#endif

#define CHECK_STREAM_WRITE_PRECOND(retVal) \
    CHECK_STREAM_PRECOND(retVal)           \
    if (mStatus != Ok) {                   \
        qWarning("Stream: status no ok");  \
        return retVal;                     \
    }

namespace qdlt645 {
Stream::Stream() {
    mDev = nullptr;
    mDevIsOwner = (false);
    mByteOrder = BigEndian;
    mNoSwap = DEFAULT_NO_SWAP;
    mStatus = Ok;
}

Stream::Stream(QIODevice *dev) {
    mDev = nullptr;
    mDevIsOwner = (false);
    mByteOrder = BigEndian;
    mNoSwap = DEFAULT_NO_SWAP;
    mStatus = Ok;
}

Stream::~Stream() {
    if (mDevIsOwner) {
        delete mDev;
    }
}

Stream::Stream(const QByteArray &data) {
    auto buffer = new QBuffer();
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);

    mDev = buffer;
    mDevIsOwner = true;
    mByteOrder = BigEndian;
    mNoSwap = DEFAULT_NO_SWAP;
    mStatus = Ok;
}

Stream::Stream(QByteArray *data, QIODevice::OpenMode flags) {
    auto buffer = new QBuffer(data);
    buffer->open(flags);

    mDev = buffer;
    mDevIsOwner = true;
    mByteOrder = BigEndian;
    mNoSwap = DEFAULT_NO_SWAP;
    mStatus = Ok;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

void Stream::setByteOrder(Stream::ByteOrder byteOrder) {
    mByteOrder = byteOrder;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        mNoSwap = (mByteOrder == BigEndian);
    } else {
        mNoSwap = (mByteOrder == LittleEndian);
    }
}

#pragma clang diagnostic pop

Stream::ByteOrder Stream::getByteOrder() const {
    return mByteOrder;
}

void Stream::setDevice(QIODevice *dev) {
    if (mDevIsOwner) {
        delete mDev;
    }
    mDev = dev;
}

QIODevice *Stream::getDevice() const {
    return mDev;
}

Stream::Status Stream::getStatus() const {
    return mStatus;
}

void Stream::setStatus(Stream::Status status) {
    if (mStatus == Ok) {
        mStatus = status;
    }
}

void Stream::resetStatus() {
    mStatus = Ok;
}

bool Stream::seek(int pos) {
    if (mDev) {
        resetStatus();
        return mDev->seek(pos);
    }
    return false;
}

Stream &Stream::operator>>(qint8 &i) {
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    char c;
    if (readBlock(&c, 1) == 1)
        i = qint8(c);
    return *this;
}

Stream &Stream::operator>>(qint16 &i) {
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    if (readBlock(reinterpret_cast<char *>(&i), 2) != 2) {
        i = 0;
    } else {
        if (!mNoSwap) {
            i = qbswap(i);
        }
    }
    return *this;
}

Stream &Stream::operator>>(qint32 &i) {
    i = 0;
    CHECK_STREAM_PRECOND(*this)
    if (readBlock(reinterpret_cast<char *>(&i), 4) != 4) {
        i = 0;
    } else {
        if (!mNoSwap) {
            i = qbswap(i);
        }
    }
    return *this;
}

Stream &Stream::operator>>(qint64 &i) {
    i = qint64(0);
    CHECK_STREAM_PRECOND(*this)
    if (readBlock(reinterpret_cast<char *>(&i), 8) != 8) {
        i = qint64(0);
    } else {
        if (!mNoSwap) {
            i = qbswap(i);
        }
    }
    return *this;
}

Stream &Stream::operator<<(qint8 i) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    if (!mDev->putChar(i)) {
        mStatus = WriteFailed;
    }
    return *this;
}

Stream &Stream::operator<<(qint16 i) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    if (!mNoSwap) {
        i = qbswap(i);
    }
    if (mDev->write((char *) &i, sizeof(qint16)) != sizeof(qint16)) {
        mStatus = WriteFailed;
    }
    return *this;
}

Stream &Stream::operator<<(qint32 i) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    if (!mNoSwap) {
        i = qbswap(i);
    }
    if (mDev->write((char *) &i, sizeof(qint32)) != sizeof(qint32)) {
        mStatus = WriteFailed;
    }

    return *this;
}

Stream &Stream::operator<<(qint64 i) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    if (!mNoSwap) {
        i = qbswap(i);
    }
    if (mDev->write((char *) &i, sizeof(qint64)) != sizeof(qint64)) {
        mStatus = WriteFailed;
    }
    return *this;
}

int Stream::readBlock(char *data, int len) {
    if (mStatus != Ok)
        return -1;
    const int readResult = mDev->read(data, len);
    if (readResult != len)
        setStatus(ReadPastEnd);
    return readResult;
}

Stream &Stream::operator<<(const Stream::FixLengthByteArray &d) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    if (d.r->length() < d.length) {
        mDev->write(*d.r);
        QByteArray fill(d.length - d.r->length(), '\0');
        mDev->write(fill);
    } else if (d.r->length() > d.length) {
        mDev->write(d.r->left(d.length));
    } else {
        mDev->write(*d.r);
    }
    return *this;
}

Stream &Stream::operator>>(const Stream::FixLengthByteArray &d) {
    CHECK_STREAM_PRECOND(*this)
    d.w->resize(d.length);
    readBlock(d.w->data(), d.length);
    return *this;
}

Stream &Stream::operator<<(const Stream::CArray &d) {
    CHECK_STREAM_WRITE_PRECOND(*this)
    mDev->write(d.r, d.length);
    return *this;
}

Stream &Stream::operator>>(const Stream::CArray &d) {
    CHECK_STREAM_PRECOND(*this)
    readBlock(d.w, d.length);
    return *this;
}


}// namespace qdlt645
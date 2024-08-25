// **************************************************
// User : qinxuchun
// Date : 2024/8/25/025 14:30
// Description : ...
//
// **************************************************

#ifndef STREAM_H
#define STREAM_H
#include "QDLT645.h"

#include <QIODevice>

namespace qdlt645 {

class QDLT645_EXPORT Stream {
public:
    enum ByteOrder {
        BigEndian = QSysInfo::BigEndian,
        LittleEndian = QSysInfo::LittleEndian
    };

    enum Status {
        Ok,
        ReadPastEnd,
        ReadCorruptData,
        WriteFailed
    };

    struct FixLengthByteArray {
        QByteArray *w;
        const QByteArray *r;
        int length;

        FixLengthByteArray(QByteArray *data, int length) :
            w(data), r(data), length(length) {}

        FixLengthByteArray(const QByteArray *data, int length) :
            w(nullptr), r(data), length(length) {}
    };

    struct CArray {
        char *w;
        const char *r;
        int length;

        CArray(const char *data, int length) :
            w(nullptr), r(data), length(length) {}

        CArray(char *data, int length) :
            w(data), r(data), length(length) {}
    };

    Stream();
    ~Stream();
    explicit Stream(QIODevice *dev);
    explicit Stream(const QByteArray &data);
    explicit Stream(QByteArray *data, QIODevice::OpenMode flags = QIODevice::ReadWrite);

    void setByteOrder(ByteOrder byteOrder);
    ByteOrder getByteOrder() const;

    void setDevice(QIODevice *dev);
    QIODevice *getDevice() const;

    Status getStatus() const;
    void setStatus(Status status);
    void resetStatus();

    bool seek(int pos);

    Stream &operator>>(qint8 &i);
    Stream &operator>>(quint8 &i);
    Stream &operator>>(qint16 &i);
    Stream &operator>>(quint16 &i);
    Stream &operator>>(qint32 &i);
    inline Stream &operator>>(quint32 &i);
    Stream &operator>>(qint64 &i);
    Stream &operator>>(quint64 &i);


    Stream &operator<<(qint8 i);
    Stream &operator<<(quint8 i);
    Stream &operator<<(qint16 i);
    Stream &operator<<(quint16 i);
    Stream &operator<<(qint32 i);
    inline Stream &operator<<(quint32 i);
    Stream &operator<<(qint64 i);
    Stream &operator<<(quint64 i);

    Stream &operator<<(const FixLengthByteArray &d);
    Stream &operator>>(const FixLengthByteArray &d);
    Stream &operator<<(const CArray &d);
    Stream &operator>>(const CArray &d);

private:
    int readBlock(char *data, int len);

private:
    Q_DISABLE_COPY(Stream)
    QIODevice *mDev;
    bool mDevIsOwner;
    ByteOrder mByteOrder;
    Status mStatus;
    bool mNoSwap;
};

inline Stream &Stream::operator>>(quint8 &i) { return *this >> reinterpret_cast<qint8 &>(i); }

inline Stream &Stream::operator>>(quint16 &i) { return *this >> reinterpret_cast<qint16 &>(i); }

inline Stream &Stream::operator>>(quint32 &i) { return *this >> reinterpret_cast<qint32 &>(i); }

inline Stream &Stream::operator>>(quint64 &i) { return *this >> reinterpret_cast<qint64 &>(i); }

inline Stream &Stream::operator<<(quint8 i) { return *this << qint8(i); }

inline Stream &Stream::operator<<(quint16 i) { return *this << qint16(i); }

inline Stream &Stream::operator<<(quint32 i) { return *this << qint32(i); }

inline Stream &Stream::operator<<(quint64 i) { return *this << qint64(i); }

}// namespace qdlt645

#endif//STREAM_H

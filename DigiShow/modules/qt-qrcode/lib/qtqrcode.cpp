/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 *
 *    Contact: daniel.samrocha@gmail.com
 *       File: qtqrcode.cpp
 *     Author: daniel
 * Created on: 03/02/2015
 *    Version:
 *
 * This file is part of the Qt QRCode library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 *
 ***************************************************************************/

#include "qtqrcode.h"

#include <QString>

int discardNegativeNumber(int number) {
    if (number < 0)
        return 0;
    return number;
}

class QtQrCodeData : public QSharedData
{
    friend class QtQrCode;
private:
    int width;
    int version;
    QByteArray data;
    QtQrCode::EncodeMode encodeMode;
    QtQrCode::Proportion proportion;
    Qt::CaseSensitivity caseSensitivity;
    QtQrCode::ErrorCorrectionLevel errorCorrectionLevel;
};

QtQrCode::QtQrCode(int version) : d(new QtQrCodeData)
{
    constructorPrivate("", version);
}

QtQrCode::QtQrCode(QtQrCode::EncodeMode encodeMod) : d(new QtQrCodeData)
{
    constructorPrivate("", 0, encodeMod);
}

QtQrCode::QtQrCode(const QByteArray &data, int version, QtQrCode::EncodeMode encodeMode,
                   QtQrCode::Proportion proportion, Qt::CaseSensitivity caseSensitivity,
                   QtQrCode::ErrorCorrectionLevel errorCorrectionLevel)
    : d(new QtQrCodeData)
{
    constructorPrivate(data, version, encodeMode, proportion,caseSensitivity, errorCorrectionLevel);
}

void QtQrCode::constructorPrivate(const QByteArray &data, int version, QtQrCode::EncodeMode encodeMode,
                             QtQrCode::Proportion proportion, Qt::CaseSensitivity caseSensitivity,
                             QtQrCode::ErrorCorrectionLevel errorCorrectionLevel)
{
    d->width = 0;
    d->data = data;
    d->version = (version > 4) ? 0 : discardNegativeNumber(version);
    d->encodeMode = encodeMode;
    d->proportion = proportion;
    d->caseSensitivity = caseSensitivity;
    d->errorCorrectionLevel = errorCorrectionLevel;
}

QtQrCode::QtQrCode(const QtQrCode &rhs) : d(rhs.d)
{
}

QtQrCode &QtQrCode::operator=(const QtQrCode &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

QtQrCode::~QtQrCode()
{
}

void QtQrCode::encode()
{
    if (d->data.isEmpty())
        return;
    switch (d->encodeMode) {
    case StringMode:
        encodeString();
        break;
    case DataMode:
        encodeData();
        break;
    case KanjiMode:
        encodeSKanji();
    default:
        break;
    }
}

void QtQrCode::setBaseQrCodeData(QRcode *qrCode)
{
    d->width = qrCode->width;
    d->data.clear();
    // TODO: Beware it here
    d->data = (const char *) qrCode->data;
    QRcode_free(qrCode);
}

void QtQrCode::encodeData()
{
    QRcode *qrCode = NULL;
    if (d->proportion == MicroProportion) {
        qrCode = QRcode_encodeDataMQR(d->data.size(), (const unsigned char *) d->data.constData(),
                                      d->version, (QRecLevel) d->errorCorrectionLevel);
    } else {
        qrCode = QRcode_encodeData(d->data.size(), (const unsigned char *) d->data.constData(),
                                   d->version, (QRecLevel) d->errorCorrectionLevel);
    }
    if (qrCode == NULL)
        return;

    setBaseQrCodeData(qrCode);
}

void QtQrCode::encodeString()
{
    QRcode *qrCode = NULL;
    if (d->proportion == MicroProportion) {
        qrCode = QRcode_encodeStringMQR(QString(d->data).toStdString().c_str(),
                                        d->version, (QRecLevel) d->errorCorrectionLevel,
                                        QR_MODE_8, d->caseSensitivity);
    } else {
        qrCode = QRcode_encodeString(QString(d->data).toStdString().c_str(),
                                     d->version, (QRecLevel) d->errorCorrectionLevel,
                                     QR_MODE_8, d->caseSensitivity);
    }
    if (qrCode == NULL)
        return;

    setBaseQrCodeData(qrCode);
}

void QtQrCode::encodeSKanji()
{
    QRcode *qrCode = NULL;
    if (d->proportion == MicroProportion) {
        qrCode = QRcode_encodeStringMQR(d->data.constData(), d->version,
                                        (QRecLevel) d->errorCorrectionLevel,
                                        QR_MODE_KANJI, d->caseSensitivity);
    } else {
        qrCode = QRcode_encodeString(d->data.constData(), d->version,
                                     (QRecLevel) d->errorCorrectionLevel,
                                     QR_MODE_KANJI, d->caseSensitivity);
    }
    if (qrCode == NULL)
        return;

    setBaseQrCodeData(qrCode);
}

int QtQrCode::width() const
{
     return d->width;
}

int QtQrCode::version() const
{
     return d->version;
}

void QtQrCode::setVersion(int version)
{
    if (version != d->version) {
        if (version > 4)
            d->version = 0;
        else
            d->version = discardNegativeNumber(version);
        this->encode();
    }
}

QByteArray QtQrCode::data() const
{
     return d->data;
}

void QtQrCode::setData(const QByteArray &data)
{
    if (data != d->data) {
        d->data = data;
        this->encode();
    }
}

QtQrCode::EncodeMode QtQrCode::encodeMode() const
{
     return d->encodeMode;
}

void QtQrCode::setEncodeMode(EncodeMode encodeMode)
{
    if (encodeMode != d->encodeMode) {
        d->encodeMode = encodeMode;
        this->encode();
    }
}

QtQrCode::Proportion QtQrCode::proportion() const
{
     return d->proportion;
}

void QtQrCode::setProportion(Proportion proportion)
{
    if (proportion != d->proportion) {
        d->proportion = proportion;
        this->encode();
    }
}

Qt::CaseSensitivity QtQrCode::caseSensitivity() const
{
     return d->caseSensitivity;
}

void QtQrCode::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (caseSensitivity != d->caseSensitivity) {
        d->caseSensitivity = caseSensitivity;
        this->encode();
    }
}

QtQrCode::ErrorCorrectionLevel QtQrCode::errorCorrectionLevel() const
{
     return d->errorCorrectionLevel;
}

void QtQrCode::setErrorCorrectionLevel(ErrorCorrectionLevel errorCorrectionLevel)
{
    if (errorCorrectionLevel != d->errorCorrectionLevel) {
        d->errorCorrectionLevel = errorCorrectionLevel;
        this->encode();
    }
}

bool QtQrCode::operator ==(const QtQrCode &other)
{
    return d->data == other.d->data;
}

bool QtQrCode::operator !=(const QtQrCode &other)
{
    return !(*this == other);
}

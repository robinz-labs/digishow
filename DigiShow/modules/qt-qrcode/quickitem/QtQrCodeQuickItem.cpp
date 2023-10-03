/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 *
 *    Contact: daniel.samrocha@gmail.com
 *       File: QtQrCodeQuickItem.cpp
 *     Author: daniel
 * Created on: 04/02/2015
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

#include "QtQrCodeQuickItem.hpp"

#include <QPainter>

QtQrCodeQuickItem::QtQrCodeQuickItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
}

QtQrCodeQuickItem::~QtQrCodeQuickItem()
{
}

void QtQrCodeQuickItem::registerQmlTypes()
{
    qmlRegisterType<QtQrCodeQuickItem>("QtQrCode.Component", 1, 0, "QtQrCode");
}

void QtQrCodeQuickItem::paint(QPainter *painter)
{
    m_qrCodePainter.paint(m_qrCode, *painter, width(), height());
}

QByteArray QtQrCodeQuickItem::data() const
{
    return m_qrCode.data();
}

void QtQrCodeQuickItem::setData(const QByteArray &data)
{
    if(m_qrCode.data() != data) {
        m_qrCode.setData(data);
        update();
        Q_EMIT this->dataChanged(m_qrCode.data());
    }
}

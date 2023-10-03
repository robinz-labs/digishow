/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 *
 *    Contact: daniel.samrocha@gmail.com
 *       File: qtqrcodepainter.cpp
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

#include "qtqrcodepainter.h"

#include <QSize>
#include <QRectF>
#include <QSvgGenerator>

QtQrCodePainter::QtQrCodePainter(float margin, const QBrush &background, const QBrush &foreground)
    : m_margin(margin), m_offsetX(0.0), m_offsetY(0.0), m_svgPaint(false),
      m_background(background), m_foreground(foreground)
{
}

QtQrCodePainter::~QtQrCodePainter()
{
}

void QtQrCodePainter::paint(const QtQrCode &qrCode, QPainter &painter)
{
    this->paint(qrCode, painter, qrCode.width() + m_margin * 2);
}

void QtQrCodePainter::paint(const QtQrCode &qrCode, QPainter &painter, int width, int height)
{
    m_offsetX = 0.0;
    m_offsetY = 0.0;
    int painterWidt = 0;
    float aspect = width/(float) height;
    if (aspect > 1.0) {
        painterWidt = height;
        m_offsetX = width/2.0 - painterWidt/2.0;
    } else {
        painterWidt = width;
        m_offsetY = height/2.0 - painterWidt/2.0;
    }
    this->paint(qrCode, painter, painterWidt);
}

void QtQrCodePainter::paint(const QtQrCode &qrCode, QPainter &painter, int painterWidth)
{
    if (qrCode.data().isEmpty())
        return;
    int width = qrCode.width();
    float aux = (m_svgPaint) ? 0.05 : 0.0;
    unsigned char *data = (unsigned char *) qrCode.data().constData();
    if (width < 0)
        width = 1;
    float scale = (painterWidth - 2.0*m_margin)/ (float) width;
    QRectF rectangle(m_offsetX, m_offsetY, painterWidth, painterWidth);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_foreground);
    painter.setClipRect(rectangle);
    painter.fillRect(rectangle, m_background);

    for(int y = 0; y < width; ++y) {
        for(int x = 0; x < width; ++x, ++data) {
            if(*data & 0x1)
                painter.drawRect(QRectF(m_offsetX + m_margin + x*scale - aux,
                                        m_offsetY + m_margin + y*scale - aux,
                                        scale + 2*aux, scale + 2*aux));
        }
    }
    m_svgPaint = false;
}

QImage QtQrCodePainter::toImage(const QtQrCode &qrCode, int size)
{
    if (size < 0 || qrCode.data().isEmpty())
        return QImage();

    int width = qrCode.width() + m_margin * 2;
    QImage img(width, width, QImage::Format_ARGB32);

    QPainter painter(&img);
    this->paint(qrCode, painter);

    if (size > 0)
        return img.scaled(size, size);
    return img;
}

bool QtQrCodePainter::saveSvg(const QtQrCode &qrCode, const QString &fileName, int size)
{
    if (fileName.isEmpty() || size < 0 || qrCode.data().isEmpty())
        return false;

    QSvgGenerator svgGenerator;
    svgGenerator.setFileName(fileName);
    svgGenerator.setSize(QSize(size, size));
    int width = qrCode.width() + m_margin * 2;
    svgGenerator.setViewBox(QRectF(0, 0, width, width));

    m_svgPaint = true;
    QPainter painter(&svgGenerator);
    this->paint(qrCode, painter);

    return true;
}

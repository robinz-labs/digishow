/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 *
 *    Contact: daniel.samrocha@gmail.com
 *       File: qtqrcodepainter.h
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

#ifndef QRCODEPAINTER_H
#define QRCODEPAINTER_H

#include <QImage>
#include <QBrush>
#include <QPainter>

#include "qtqrcode.h"

class QtQrCodePainter
{
public:
    QtQrCodePainter(float margin = 0.0, const QBrush &background = Qt::white,
                    const QBrush &foreground = Qt::black);
    ~QtQrCodePainter();

    void paint(const QtQrCode &qrCode, QPainter &painter);
    void paint(const QtQrCode &qrCode, QPainter &painter, int width, int height);
    void paint(const QtQrCode &qrCode, QPainter &painter, int painterWidth);
    QImage toImage(const QtQrCode &qrCode, int size);
    bool saveSvg(const QtQrCode &qrCode, const QString &fileName, int size);

    inline float margin() const { return m_margin; }
    inline  bool setMargin(float margin)
    {
        if (m_margin != margin) {
            m_margin = margin;
            return true;
        }
        return false;
    }

    inline const QBrush &background() const { return m_background; }
    inline  bool setBackground(const QBrush &background)
    {
        if (m_background != background) {
            m_background = background;
            return true;
        }
        return false;
    }

    inline const QBrush &foreground() const { return m_foreground; }
    inline  bool setForeground(const QBrush &foreground)
    {
        if (m_foreground != foreground) {
            m_foreground = foreground;
            return true;
        }
        return false;
    }

private:
    float m_margin;
    float m_offsetX;
    float m_offsetY;
    bool m_svgPaint;
    QBrush m_background;
    QBrush m_foreground;
};

#endif // QRCODEPAINTER_H

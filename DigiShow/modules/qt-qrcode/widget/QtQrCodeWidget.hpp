/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 *
 *    Contact: daniel.samrocha@gmail.com
 *       File: QtQrCodeWidget.cpp
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

#ifndef QTQRCODEWIDGET_HPP
#define QTQRCODEWIDGET_HPP

#include <QColor>
#include <QWidget>
#include <QByteArray>

#include <QtQrCode>
#include <QtQrCodePainter>

QT_BEGIN_NAMESPACE
class QPaintEvent;
QT_END_NAMESPACE

class QtQrCodeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(float margin READ margin WRITE setMargin NOTIFY marginChanged)
    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)
public:
    explicit QtQrCodeWidget(QWidget *parent = 0);
    ~QtQrCodeWidget();

    inline float margin() const { return m_qrCodePainter.margin(); }
    inline virtual void setMargin(float margin)
    {
        if (m_qrCodePainter.setMargin(margin)) {
            QWidget::repaint();
            emit marginChanged(m_qrCodePainter.margin());
        }
    }

    inline const QColor &background() const { return m_qrCodePainter.background().color(); }
    inline virtual void setBackground(QColor backgroundColor)
    {
        if (m_qrCodePainter.setBackground(backgroundColor)) {
            QWidget::repaint();
            emit backgroundChanged(m_qrCodePainter.background());
        }
    }

    inline const QColor &foreground() const { return m_qrCodePainter.foreground().color(); }
    inline virtual void setForeground(QColor foregroundColor)
    {
        if (m_qrCodePainter.setForeground(foregroundColor)) {
            QWidget::repaint();
            emit foregroundChanged(m_qrCodePainter.foreground());
        }
    }

    QByteArray data() const;
    void setData(const QByteArray &data);
signals:
    void dataChanged(const QByteArray &data);
    void marginChanged(int margin);
    void backgroundChanged(const QBrush &background);
    void foregroundChanged(const QBrush &foreground);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QtQrCode m_qrCode;
    QtQrCodePainter m_qrCodePainter;
};

#endif // QTQRCODEWIDGET_HPP

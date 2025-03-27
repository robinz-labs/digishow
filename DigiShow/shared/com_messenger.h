/*
    Copyright 2025 Robin Zhang & Labs

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
    NOTE: 
    The code in this file was co-written by AI (Trae/Claude-3.5-Sonnet).
*/

#ifndef COM_MESSENGER_H
#define COM_MESSENGER_H

#include "abstract_messenger.h"
#include <QSerialPort>

class ComMessenger : public AbstractMessenger
{
    Q_OBJECT

public:
    // Serial port settings
    enum SerialSetting {
        Setting8N1 = 0,
        Setting8E1 = 1,
        Setting8O1 = 2,
        Setting7E1 = 3,
        Setting7O1 = 4,
        Setting8N2 = 5
    };
    Q_ENUM(SerialSetting)

    explicit ComMessenger(QObject *parent = nullptr);
    ~ComMessenger();

    bool open(const QString &portName, 
             int portBaud = QSerialPort::Baud115200,
             SerialSetting portSetting = Setting8N1);
    void close() override;
    bool sendMessage(const QByteArray &message) override;

private slots:
    void handleReadyRead();

private:
    QSerialPort *m_serialPort;
    QByteArray m_buffer;
};

#endif // COM_MESSENGER_H
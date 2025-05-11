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

#include "com_messenger.h"

// Constructor: Initialize serial port
ComMessenger::ComMessenger(QObject *parent)
    : AbstractMessenger(parent)
    , m_serialPort(new QSerialPort(this))
    , m_rawDataTimer(new QTimer(this))
{
    // Set timer for raw data buffering
    m_rawDataTimer->setSingleShot(true);
    m_rawDataTimer->setInterval(50);
    
    // Connect timer to emit raw data signal
    connect(m_rawDataTimer, &QTimer::timeout, this, [this]() {
        if (!m_rawDataBuffer.isEmpty()) {
            emit rawDataReceived(m_rawDataBuffer);
            m_rawDataBuffer.clear();
        }
    });

    // Connect the signal to handle incoming data
    connect(m_serialPort, &QSerialPort::readyRead, this, &ComMessenger::handleReadyRead);
}

// Destructor: Ensure proper cleanup
ComMessenger::~ComMessenger()
{
    close();
}

// Open serial port with specified parameters
bool ComMessenger::open(const QString &portName, int portBaud, SerialSetting portSetting)
{
    close();
    m_serialPort->setPortName(portName);
    
    // First open the port
    bool success = m_serialPort->open(QIODevice::ReadWrite);
    if (!success) return false;

    // Then configure port settings
    m_serialPort->setBaudRate(portBaud);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // Configure port settings based on SerialSetting enum
    switch (portSetting) {
        case Setting8N1:
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::NoParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case Setting8E1:
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::EvenParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case Setting8O1:
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::OddParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case Setting7E1:
            m_serialPort->setDataBits(QSerialPort::Data7);
            m_serialPort->setParity(QSerialPort::EvenParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case Setting7O1:
            m_serialPort->setDataBits(QSerialPort::Data7);
            m_serialPort->setParity(QSerialPort::OddParity);
            m_serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case Setting8N2:
            m_serialPort->setDataBits(QSerialPort::Data8);
            m_serialPort->setParity(QSerialPort::NoParity);
            m_serialPort->setStopBits(QSerialPort::TwoStop);
            break;
    }

    emit connected();
    return true;
}

// Close the serial port
void ComMessenger::close()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        emit disconnected();
    }
}

// Send message through serial port
bool ComMessenger::sendMessage(const QByteArray &message)
{
    if (!m_serialPort->isOpen()) {
        return false;
    }
    
    qint64 written = m_serialPort->write(message);
    return written == message.size();
}

// Handle incoming serial data
void ComMessenger::handleReadyRead()
{
    // Accumulate received raw data in buffer
    QByteArray newData = m_serialPort->readAll();
    m_rawDataBuffer.append(newData);
    // Start timer to emit raw data signal after a delay for buffering the complete message
    m_rawDataTimer->start(); 
    
    // Process subscribed messages
    m_buffer.append(newData);

    bool found = true;
    while (found) {
        found = false;
        int earliestPos = m_buffer.size();
        int foundIndex = -1;

        // Find the earliest message in buffer
        for (int i = 0; i < m_subscribedMessages.size(); ++i) {
            const QByteArray &message = m_subscribedMessages[i];
            int pos = m_buffer.indexOf(message);
            if (pos != -1 && pos < earliestPos) {
                earliestPos = pos;
                foundIndex = i;
                found = true;
            }
        }

        // Process the earliest message if found
        if (found) {
            emit messageReceived(foundIndex);
            const QByteArray &message = m_subscribedMessages[foundIndex];
            m_buffer.remove(earliestPos, message.size());
        }
    }

    // Prevent buffer from growing indefinitely
    if (m_buffer.size() > 1024) {
        m_buffer.remove(0, m_buffer.size() - 1024);
    }
}

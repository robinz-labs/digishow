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

#ifndef ABSTRACT_MESSAGER_H
#define ABSTRACT_MESSAGER_H

#include <QObject>
#include <QByteArray>
#include <QList>

class AbstractMessager : public QObject
{
    Q_OBJECT

public:
    explicit AbstractMessager(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~AbstractMessager() {}

    virtual void close() = 0;
    virtual bool sendMessage(const QByteArray &message) = 0;

    virtual int subscribeMessage(const QByteArray &message) {
        m_subscribedMessages.append(message);
        return m_subscribedMessages.size() - 1;
    }

    const QList<QByteArray>& subscribedMessages() const { return m_subscribedMessages; }

signals:
    void messageReceived(int index);
    void rawDataReceived(const QByteArray &data);
    void connected();
    void disconnected();

protected:
    QList<QByteArray> m_subscribedMessages;
};

#endif // ABSTRACT_MESSAGER_H
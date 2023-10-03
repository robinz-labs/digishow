/*
    Copyright 2023 Robin Zhang & Labs

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

#ifndef DIGISHOWCLOUD_H
#define DIGISHOWCLOUD_H

#include <QVariant>

class DigishowCloud : public QObject
{
    Q_OBJECT

public:

    enum SignInStatus {
        SignInPending = 0,
        SignInSuccess = 1,
        SignInError   = 2
    };
    Q_ENUM(SignInStatus)

    explicit DigishowCloud(QObject *parent = nullptr);
    ~DigishowCloud();

    // for cloud sign in
    Q_INVOKABLE QString getSignInCode();      // to generate the qr code
    Q_INVOKABLE int checkSignInStatus();      // to determine whether the qr code has been scanned by a phone app

    Q_INVOKABLE QVariantMap getSessionInfo(); // obtain the session information after signed in
    Q_INVOKABLE bool signOut();               // sign out from the cloud


    // for cloud pipe
    Q_INVOKABLE QString getCloudPipeUrl(const QString &pipeId); // to set up a websocket pipe to the cloud

signals:

};

#endif // DIGISHOWCLOUD_H

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

#include "digishow_cloud_dummy.h"
#include "app_utilities.h"



// NOTE: THERE IS ONLY A DUMMY IMPLEMENTATION !!



DigishowCloud::DigishowCloud(QObject *parent)
    : QObject{parent}
{

}

DigishowCloud::~DigishowCloud()
{

}

QString DigishowCloud::getSignInCode()
{
    // dummy
    return AppUtilities::createUUID();
}

int DigishowCloud::checkSignInStatus()
{
    // dummy
    return DigishowCloud::SignInPending;
}

QVariantMap DigishowCloud::getSessionInfo()
{
    // dummy
    QVariantMap sessionInfo;
    //sessionInfo["clientName"] = "OUTLET 1";
    //sessionInfo["clientId"  ] = "XXXXXXXXXXXXXXXX";
    //sessionInfo["sessionId" ] = "XXXXXXXXXXXXXXXX";
    return sessionInfo;
}

bool DigishowCloud::signOut()
{
    // dummy
    return true;
}

QString DigishowCloud::getCloudPipeUrl(const QString &pipeId)
{
    // dummy
    return "ws://digishow_cloud_server:50000/pipe/" + pipeId;
}

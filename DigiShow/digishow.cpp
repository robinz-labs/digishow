/*
    Copyright 2021-2025 Robin Zhang & Labs

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

#include "digishow.h"

QString g_appname   = "DigiShow LINK";
QString g_fullname  = "";
QString g_version   = "1.7.3";
QString g_copyright = "";
QString g_serial    = "";
bool g_experimental = false;

DigishowApp* g_app = nullptr;
QQmlApplicationEngine *g_engine = nullptr;

bool g_needLogCom  = false;
bool g_needLogCtl  = false;
bool g_needLogSvr  = false;

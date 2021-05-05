#include "digishow.h"

QString g_appname = "DigiShow LINK";
QString g_version = "1.1.6";

DigishowApp* g_app = nullptr;

bool g_needLogCom  = false;
bool g_needLogCtl  = false;
bool g_needLogSvr  = false;

/*

  DIGISHOW APP

  CHANGE LIST

  v1.0.0 20200617

  ! pre-released as the version 1.0
  + added windows msvc compatibility
  + got ready for app deployment on macos and windows

  v1.0.1 20200701
  - removed all program works with code-type signals
  ! rewrite screen player using new signal controls

  v1.0.2 20200730
  + added quick launch panel

  v1.0.3 20201011
  + added more options for endpoint configurations

  v1.1.0 20201012
  beta release

  v1.1.1 20201017
  beta release

  v1.1.2 20201110
  beta release

  v1.1.3 20201130
  beta release
  + allows preset launch in slot output
  + added remote pipe support (websocket interface)

 */

#ifndef DIGISHOW_H
#define DIGISHOW_H

#include <QDebug>
#include "digishow_app.h"
#include "digishow_common.h"
#include "app_utilities.h"

extern QString g_appname;
extern QString g_version;

extern DigishowApp *g_app;

extern bool g_needLogCom;
extern bool g_needLogCtl;
extern bool g_needLogSvr;

#endif // DIGISHOW_H

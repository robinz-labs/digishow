/*
    Copyright 2021-2024 Robin Zhang & Labs

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

  v1.1.5 20210110
  beta release
  + allows to transform screen canvas where presents media clips

  v1.1.6 20210505
  beta release
  + artnet support
  + smoothing analog output
  + copy-paste slot options

  v1.1.7 20210630
  beta release
  + copy-paste support
  + undo-redo support
  + japanese and spanish support

  v1.1.8 20210801
  beta release
  + bookmark support
  ! windows compatibility issue fixes

  v1.2 20211122
  stable release
  + allows new app instances for separate digishow project runtimes
  + allows to select a block of link items using the shift key
  ! fixed Arduino/RIOC servo initial positioning issue
  + display scaling support

  v1.2.1 20220420
  + osc interface support
  + midi / osc input signal learning
  + open recent files menu

  v1.2.2 20220528
  + global hotkey input interface
  + metronome and beat trigger interface

  v1.2.3 20220827
  + level meter of audio-in interface
  + in-delay / out-delay envelope parameters
  + autostart option

  v1.2.5 20230821
  + pixel player support over artnet (experimental)
  ! fixed windows filepath url issue
  + multi-session remote pipe
  ! fixed filepath url issue
  + add-on process support (experimental)
  ! code optimization for qt5.15
  ! rewrite osc interface using tinyosc library
  + remote pipe supports more messages

  v1.2.6 20231020
  ! improved preset launcher and beat maker (metronome)
  ! improved virtual pipe features
  + pixel player support for dmx and artnet
  + 16-bit dimmer support for dmx and artnet
  + allows changing osc integer value range
  + compatible dmx usb pro widget support
  + digishow cloud service support ready

  v1.2.7 20240107
  + midi pitch bend support
  ! improved arduino features
  ! improved ui

  v1.5 20240316
  alpha release
  + scriptable environment and javascript expression support
  ! complete signal mapping transformation options
  ! improved pixel mapping over artnet/dmx
  ! stability improvements

  v1.5.1 20240405
  beta release
  ! bug fixes and improvements

  v1.5.2 20240608
  final release
  ! bug fixes and improvements

  v1.6 20250218
  alpha release
  + built-in web service for remote control
  + online tutorials
  ! improved arduino rioc to support user channels etc.
  ! bug fixes and improvements

  v1.6.1 20250228
  beta release
  + sending code over udp/tcp/com by scripts
  + multi-line expression editor

  v1.6.2 20250303
  final release
  ! fixed bugs of rioc channel number and pin number conflict
  + qr code for web remote control

  v1.6.3 20250313
  ! rewrote the audio input interface with FFT

  v1.6.5 20250323
  + com/tcp/udp messenger interface
  ! improved ui

  v1.6.6 20250401
  + channel padding in dmx / artnet pixel mapping
  ! rewrote the open dmx usb support with libFTDI

  v1.7 20250510
  alpha release
  + cue player attached to preset
  + audio player interface
  ! improved code learning from serial messenger
  ! improved dmx/arntnet pixel mapping

  v1.7.2 20250605
  beta release
  ! rewrote the timeline editor UI using qml, deprecated the html version
  ! improved present launcher
  ! improved scriptable enviroment

  v1.7.3 20250616
  final release
  ! fixed master volume bug in audio player
  + added beat count and reset signals in metronome
  ! improved the ui of preset launcher
  ! improved the ui of bookmarks

  v1.7.4 20250913
  ! improved arduino stepper control
  ! improved cue player and preset launcher

  v1.7.5 20251128
  + added support for more Arduino / Aladdin PLC models
  ! fixed audio playback duration setting problem

  v1.7.6 20250214
  + auto serial reconnection
  + more video playback controls
  + file encryption support (experimental)
  + playback control of mutual exclusive cue tracks
  + both Play and Launch controls for media or cue players
  ! bug fixes

*/

#ifndef DIGISHOW_H
#define DIGISHOW_H

#include <QDebug>
#include <QQmlApplicationEngine>
#include "digishow_app.h"
#include "digishow_common.h"
#include "app_utilities.h"

extern QString g_appname;
extern QString g_fullname;
extern QString g_version;
extern QString g_copyright;
extern QString g_serial;
extern QString g_language;
extern bool g_experimental;

extern DigishowApp *g_app;
extern QQmlApplicationEngine *g_engine;

extern bool g_needLogCom;
extern bool g_needLogCtl;
extern bool g_needLogSvr;

#endif // DIGISHOW_H

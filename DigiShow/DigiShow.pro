QT += quick serialport webengine websockets multimedia

CONFIG += c++11 sdk_no_version_check

DEFINES -= QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_DEPRECATED_WARNINGS

SOURCES += \
    $$PWD/shared/md5.cpp \
    $$PWD/shared/tcp_handler.cpp \
    $$PWD/shared/com_handler.cpp \
    $$PWD/shared/modbus_rtu_handler.cpp \
    $$PWD/shared/modbus_tcp_handler.cpp \
    $$PWD/shared/rioc_service.cpp \
    $$PWD/shared/rioc_controller.cpp \
    $$PWD/shared/http_server.cpp \
    $$PWD/shared/http_socket.cpp \
    $$PWD/shared/kiss_fft.cpp \
    $$PWD/shared/audio_analyzer.cpp \
    $$PWD/shared/com_messenger.cpp \
    $$PWD/shared/tcp_messenger.cpp \
    $$PWD/shared/udp_messenger.cpp \
    app_utilities.cpp \
    dgs_midi_interface.cpp \
    dgs_rioc_interface.cpp \
    dgs_modbus_interface.cpp \
    dgs_hue_interface.cpp \
    dgs_dmx_interface.cpp \
    dgs_artnet_interface.cpp \
    dgs_osc_interface.cpp \
    dgs_audioin_interface.cpp \
    dgs_screen_interface.cpp \
    dgs_pipe_interface.cpp \
    dgs_launch_interface.cpp \
    dgs_hotkey_interface.cpp \
    dgs_metronome_interface.cpp \
    dgs_messenger_interface.cpp \
    digishow.cpp \
    digishow_app.cpp \
    digishow_cloud_dummy.cpp \
    digishow_common.cpp \
    digishow_data.cpp \
    digishow_environment.cpp \
    digishow_interface.cpp \
    digishow_remote_web.cpp \
    digishow_scriptable.cpp \
    digishow_slot.cpp \
    digishow_cue_manager.cpp \
    digishow_cue_player.cpp \
    digishow_metronome.cpp \
    digishow_pixel_player.cpp \
    main.cpp

HEADERS += \
    $$PWD/shared/md5.h \
    $$PWD/shared/tcp_handler.h \
    $$PWD/shared/com_handler.h \
    $$PWD/shared/modbus_handler.h \
    $$PWD/shared/modbus_rtu_handler.h \
    $$PWD/shared/modbus_tcp_handler.h \
    $$PWD/shared/rioc_service.h \
    $$PWD/shared/rioc_controller.h \
    $$PWD/shared/rioc_aladdin2560_def.h \
    $$PWD/shared/http_server.h \
    $$PWD/shared/http_socket.h \
    $$PWD/shared/kiss_fft.h \
    $$PWD/shared/kiss_fft_log.h \
    $$PWD/shared/_kiss_fft_guts.h \
    $$PWD/shared/audio_analyzer.h \
    $$PWD/shared/abstract_messenger.h \
    $$PWD/shared/com_messenger.h \
    $$PWD/shared/tcp_messenger.h \
    $$PWD/shared/udp_messenger.h \
    app_common.h \
    app_utilities.h \
    dgs_midi_interface.h \
    dgs_rioc_interface.h \
    dgs_modbus_interface.h \
    dgs_hue_interface.h \
    dgs_dmx_interface.h \
    dgs_artnet_interface.h \
    dgs_osc_interface.h \
    dgs_audioin_interface.h \
    dgs_screen_interface.h \
    dgs_pipe_interface.h \
    dgs_launch_interface.h \
    dgs_hotkey_interface.h \
    dgs_metronome_interface.h \
    dgs_messenger_interface.h \
    digishow.h \
    digishow_app.h \
    digishow_cloud_dummy.h \
    digishow_common.h \
    digishow_data.h \
    digishow_environment.h \
    digishow_interface.h \
    digishow_remote_web.h \
    digishow_scriptable.h \
    digishow_slot.h \
    digishow_cue_manager.h \
    digishow_cue_player.h \
    digishow_metronome.h \
    digishow_pixel_player.h

RESOURCES += $$PWD/resources/main.qrc

INCLUDEPATH += $$PWD/shared/

win32-msvc*: {
    QMAKE_CFLAGS *= /utf-8
    QMAKE_CXXFLAGS *= /utf-8
}

win32: {
    RC_ICONS = $$PWD/icons/icon.ico
}
unix:!macx {

}
macx: {
    LIBS += \
        -framework Foundation \
        -framework Cocoa \
        -framework CoreFoundation \
        -framework AVFoundation

    QMAKE_CXXFLAGS += -x objective-c++

    OBJECTIVE_SOURCES += mac_utilities.mm
    HEADERS += mac_utilities.h

    QMAKE_INFO_PLIST = Info.plist
    ICON = $$PWD/icons/app.icns
    DISTFILES += $$PWD/icons/doc.icns

    #QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
}

TRANSLATIONS = \
    $$PWD/resources/translations/language.zh_CN.ts \
    $$PWD/resources/translations/language.jp.ts \
    $$PWD/resources/translations/language.es.ts

# RtMidi library
include(modules/rtmidi/rtmidi.pri)

# TinyOSC library
include(modules/tinyosc/tinyosc.pri)

# QHotKey library
include(modules/qhotkey/qhotkey.pri)

# Ableton Link library
include(modules/ableton/link.pri)

# Qt QR Code library
include(modules/qt-qrcode/qt-qrcode.pri)

# libFTDI library
include(modules/libftdi/libftdi.pri)

# cloud implementation
#include(digishow_cloud/cloud.pri)

# experimental code
#include(digishow_experimental/experimental.pri)

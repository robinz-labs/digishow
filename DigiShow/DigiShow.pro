QT += quick serialport webengine websockets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$PWD/shared/tcp_handler.cpp \
        $$PWD/shared/com_handler.cpp \
        $$PWD/shared/modbus_rtu_handler.cpp \
        $$PWD/shared/modbus_tcp_handler.cpp \
        $$PWD/shared/rioc_service.cpp \
        $$PWD/shared/rioc_controller.cpp \
        app_utilities.cpp \
        dgs_artnet_interface.cpp \
        dgs_dmx_interface.cpp \
        dgs_midi_interface.cpp \
        dgs_modbus_interface.cpp \
        dgs_rioc_interface.cpp \
        dgs_hue_interface.cpp \
        dgs_screen_interface.cpp \
        dgs_pipe_interface.cpp \
        dgs_launch_interface.cpp \
        digishow.cpp \
        digishow_app.cpp \
        digishow_common.cpp \
        digishow_environment.cpp \
        digishow_interface.cpp \
        digishow_slot.cpp \
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
    app_common.h \
    app_utilities.h \
    dgs_artnet_interface.h \
    dgs_dmx_interface.h \
    dgs_midi_interface.h \
    dgs_modbus_interface.h \
    dgs_rioc_interface.h \
    dgs_hue_interface.h \
    dgs_screen_interface.h \
    dgs_pipe_interface.h \
    dgs_launch_interface.h \
    digishow.h \
    digishow_app.h \
    digishow_common.h \
    digishow_environment.h \
    digishow_interface.h \
    digishow_slot.h

RESOURCES += $$PWD/resources/main.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: {
    INCLUDEPATH += $$PWD/../rtmidi/include
    LIBS += -L$$PWD/../rtmidi/lib/win -lrtmidi

    RC_ICONS = $$PWD/icons/icon.ico
}
unix:!macx {

}
macx: {
    INCLUDEPATH += $$PWD/../rtmidi/include
    LIBS += -L$$PWD/../rtmidi/lib/mac -lrtmidi

    LIBS += -framework CoreMIDI \
            -framework CoreFoundation

    INCLUDEPATH += /System/Library/Frameworks/AppKit.framework/Headers \
                   /System/Library/Frameworks/Cocoa.framework/Headers
    LIBS += -framework Foundation \
            -framework Cocoa

    OBJECTIVE_SOURCES += mac_utilities.mm
    HEADERS += mac_utilities.h

    ICON = $$PWD/icons/icon.icns
    QMAKE_INFO_PLIST = Info.plist
}

TRANSLATIONS = \
    $$PWD/resources/translations/language.zh_CN.ts \
    $$PWD/resources/translations/language.jp.ts



CONFIG += c++11

win32: {
    DEFINES += __WINDOWS_MM__
    LIBS += -lwinmm
}
unix:!macx {
    DEFINES += __LINUX_ALSA__
    LIBS += -lasound
}
macx: {
    DEFINES += __MACOSX_CORE__
    LIBS += -framework CoreMIDI -framework CoreAudio
}

INCLUDEPATH += \
    $$PWD \
    $$PWD/..

SOURCES += \
    $$PWD/rtmidi/RtMidi.cpp

HEADERS += \
    $$PWD/rtmidi/RtMidi.h


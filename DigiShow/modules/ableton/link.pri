CONFIG += c++11

win32: {
DEFINES += LINK_PLATFORM_WINDOWS
}
unix:!macx {
DEFINES += LINK_PLATFORM_LINUX LINK_PLATFORM_UNIX
}
macx: {
DEFINES += LINK_PLATFORM_MACOSX LINK_PLATFORM_UNIX
}

INCLUDEPATH += \
    $$PWD/link/include \
    $$PWD/link/modules/asio-standalone/asio/include

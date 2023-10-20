QT += svg

INCLUDEPATH += \
    $$PWD \
    $$PWD/lib \
    $$PWD/quickitem \
    $$PWD/..

SOURCES += \
    $$PWD/lib/libqrencode/bitstream.c \
    $$PWD/lib/libqrencode/qrencode.c \
    $$PWD/lib/libqrencode/mqrspec.c \
    $$PWD/lib/libqrencode/qrinput.c \
    $$PWD/lib/libqrencode/qrspec.c \
    $$PWD/lib/libqrencode/split.c \
    $$PWD/lib/libqrencode/rsecc.c \
    $$PWD/lib/libqrencode/mmask.c \
#   $$PWD/lib/libqrencode/qrenc.c \
    $$PWD/lib/libqrencode/mask.c

HEADERS += \
    $$PWD/lib/libqrencode/qrencode_inner.h \
    $$PWD/lib/libqrencode/bitstream.h \
    $$PWD/lib/libqrencode/qrencode.h \
    $$PWD/lib/libqrencode/mqrspec.h \
    $$PWD/lib/libqrencode/qrinput.h \
    $$PWD/lib/libqrencode/qrspec.h \
    $$PWD/lib/libqrencode/split.h \
    $$PWD/lib/libqrencode/rsecc.h \
    $$PWD/lib/libqrencode/mmask.h \
    $$PWD/lib/libqrencode/mask.h


SOURCES += \
    $$PWD/lib/qtqrcode.cpp \
    $$PWD/lib/qtqrcodepainter.cpp

HEADERS += \
    $$PWD/lib/qtqrcode_global.h \
    $$PWD/lib/qtqrcode.h \
    $$PWD/lib/qtqrcodepainter.h


SOURCES += \
    $$PWD/quickitem/QtQrCodeQuickItem.cpp

HEADERS += \
    $$PWD/quickitem/QtQrCodeQuickItem.hpp


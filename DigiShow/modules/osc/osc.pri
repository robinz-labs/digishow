QT += network

CONFIG += c++11

INCLUDEPATH += \
        $$PWD \
        $$PWD/..

SOURCES += \
        $$PWD/osc/composer/OscBundleComposer.cpp \
        $$PWD/osc/composer/OscContentComposer.cpp \
        $$PWD/osc/composer/OscMessageComposer.cpp \
        $$PWD/osc/OscPatternMatching.cpp \
        $$PWD/osc/reader/OscBundle.cpp \
        $$PWD/osc/reader/OscContent.cpp \
        $$PWD/osc/reader/OscMessage.cpp \
        $$PWD/osc/reader/OscReader.cpp \
        $$PWD/osc/reader/OscValue.cpp \
        $$PWD/osc/reader/types/OscAddress.cpp \
        $$PWD/osc/reader/types/OscArrayBegin.cpp \
        $$PWD/osc/reader/types/OscArrayEnd.cpp \
        $$PWD/osc/reader/types/OscBlob.cpp \
        $$PWD/osc/reader/types/OscChar.cpp \
        $$PWD/osc/reader/types/OscDouble.cpp \
        $$PWD/osc/reader/types/OscFalse.cpp \
        $$PWD/osc/reader/types/OscFloat.cpp \
        $$PWD/osc/reader/types/OscInfinitum.cpp \
        $$PWD/osc/reader/types/OscInteger.cpp \
        $$PWD/osc/reader/types/OscLong.cpp \
        $$PWD/osc/reader/types/OscMidi.cpp \
        $$PWD/osc/reader/types/OscNil.cpp \
        $$PWD/osc/reader/types/OscRGBA.cpp \
        $$PWD/osc/reader/types/OscString.cpp \
        $$PWD/osc/reader/types/OscSymbol.cpp \
        $$PWD/osc/reader/types/OscTags.cpp \
        $$PWD/osc/reader/types/OscTimeTag.cpp \
        $$PWD/osc/reader/types/OscTrue.cpp \
        $$PWD/tools/ByteBuffer.cpp \
        $$PWD/tools/ByteOrder.cpp \
        $$PWD/tools/NtpTimestamp.cpp

HEADERS += \
        $$PWD/osc/composer/OscBundleComposer.h \
        $$PWD/osc/composer/OscContentComposer.h \
        $$PWD/osc/composer/OscMessageComposer.h \
        $$PWD/osc/exceptions/BooleanConversionException.h \
        $$PWD/osc/exceptions/BytesConversionException.h \
        $$PWD/osc/exceptions/CharConversionException.h \
        $$PWD/osc/exceptions/DoubleConversionException.h \
        $$PWD/osc/exceptions/FloatConversionException.h \
        $$PWD/osc/exceptions/GetBundleException.h \
        $$PWD/osc/exceptions/GetMessageException.h \
        $$PWD/osc/exceptions/IntegerConversionException.h \
        $$PWD/osc/exceptions/LongConversionException.h \
        $$PWD/osc/exceptions/MalformedArrayException.h \
        $$PWD/osc/exceptions/MalformedBundleException.h \
        $$PWD/osc/exceptions/MidiConversionException.h \
        $$PWD/osc/exceptions/OSC_ALL_EXCEPTIONS.h \
        $$PWD/osc/exceptions/OutOfBoundsReadException.h \
        $$PWD/osc/exceptions/ReadMessageException.h \
        $$PWD/osc/exceptions/RgbaConversionException.h \
        $$PWD/osc/exceptions/StringConversionException.h \
        $$PWD/osc/exceptions/SymbolConversionException.h \
        $$PWD/osc/exceptions/TimetagConversionException.h \
        $$PWD/osc/exceptions/UnknownTagException.h \
        $$PWD/osc/OscAPI.h \
        $$PWD/osc/OscPatternMatching.h \
        $$PWD/osc/OscVersion.h \
        $$PWD/osc/reader/OscBundle.h \
        $$PWD/osc/reader/OscContent.h \
        $$PWD/osc/reader/OscMessage.h \
        $$PWD/osc/reader/OscReader.h \
        $$PWD/osc/reader/types/Midi.h \
        $$PWD/osc/reader/types/OscAddress.h \
        $$PWD/osc/reader/types/OscArrayBegin.h \
        $$PWD/osc/reader/types/OscArrayEnd.h \
        $$PWD/osc/reader/types/OscBlob.h \
        $$PWD/osc/reader/types/OscChar.h \
        $$PWD/osc/reader/types/OscDouble.h \
        $$PWD/osc/reader/types/OscFalse.h \
        $$PWD/osc/reader/types/OscFloat.h \
        $$PWD/osc/reader/types/OscInfinitum.h \
        $$PWD/osc/reader/types/OscInteger.h \
        $$PWD/osc/reader/types/OscLong.h \
        $$PWD/osc/reader/types/OscMidi.h \
        $$PWD/osc/reader/types/OscNil.h \
        $$PWD/osc/reader/types/OscRGBA.h \
        $$PWD/osc/reader/types/OscString.h \
        $$PWD/osc/reader/types/OscSymbol.h \
        $$PWD/osc/reader/types/OscTags.h \
        $$PWD/osc/reader/types/OscTimetag.h \
        $$PWD/osc/reader/types/OscTrue.h \
        $$PWD/osc/reader/types/OscValue.h \
        $$PWD/osc/reader/types/RGBA.h \
        $$PWD/osc/reader/types/Symbol.h \
        $$PWD/tools/ByteBuffer.h \
        $$PWD/tools/ByteOrder.h \
        $$PWD/tools/exceptions/BufferOverflowException.h \
        $$PWD/tools/exceptions/BufferUnderflowException.h \
        $$PWD/tools/exceptions/IllegalArgumentException.h \
        $$PWD/tools/exceptions/IndexOutOfBoundsException.h \
        $$PWD/tools/NtpTimestamp.h

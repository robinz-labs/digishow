/*
 * Copyright (c) 2014 MUGEN SAS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include <osc/reader/types/OscValue.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/TimetagConversionException.h>
#include <osc/exceptions/MidiConversionException.h>
#include <osc/exceptions/RgbaConversionException.h>

OscValue::OscValue(char typeTag, ByteBuffer* packet, qint32 pos)
: mPacket(packet)
, mTag(typeTag)
, mPos(pos)
{
}

char OscValue::getTag()
{
	return mTag;
}

/** */
bool OscValue::isArrayBegin()
{
	return false;
}

/** */
bool OscValue::isArrayEnd()
{
	return false;
}

/** */
bool OscValue::isFalse()
{
	return false;
}

/** */
bool OscValue::isInfinitum()
{
	return false;
}

/** */
bool OscValue::isNil()
{
	return false;
}

/** */
bool OscValue::isTrue()
{
	return false;
}

/** */
bool OscValue::toBoolean()
{
	return false;
}

/** */
char OscValue::toChar()
{
	return getTag();
}

/**
 * Returns the position of the next null character in the packet.
 *
 * @return the position.
 */
qint32 OscValue::getLastStringIdx(ByteBuffer* packet)
{
	// only check string for last 4th byte
	qint32 pos = packet->getPosition();
	while (packet->get(pos) != 0)
		pos += 1;
	return pos;
}

/**
 * Returns the string located at the passed position in the packet buffer.
 *
 * @param pos The position were the string starts
 * @return The read string.
 */
QByteArray OscValue::getString(ByteBuffer* packet, qint32 pos)
{
	try
	{
		packet->setPosition(pos);
		qint32 end = getLastStringIdx(packet);		
        qint32 size = end - pos;
        QByteArray bytes(size, 0);
        packet->get(&bytes, 0, size);

        qint32 rem = 4 - ((size+1) % 4);
        if (rem == 4)
            rem = 0;

        packet->setPosition(pos + size + 1 + rem);

		return bytes;
	}
	catch (const QException& e)
	{
		throw e;
	}
}

QString OscValue::toString()
{
	return QString(getTag());
}

QString OscValue::toSymbol()
{
	return QString(getTag());
}

qint32 OscValue::toInteger()
{
	return (qint32)toLong();
}

float OscValue::toFloat()
{
	return (float)toDouble();
}

Midi OscValue::toMIDI()
{
	throw MidiConversionException();
}

RGBA OscValue::toRGBA()
{
	throw RgbaConversionException();
}

QDateTime OscValue::toTimetag()
{
	throw TimetagConversionException();
}


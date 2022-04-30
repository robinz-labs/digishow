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
 
#include <osc/reader/types/OscLong.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <limits>
#include <tools/NtpTimestamp.h>

OscLong::OscLong(ByteBuffer* packet, qint32 pos)
: OscValue('h', packet, pos)
{
	mPacket->getLong();
}

/**
 * Returns the int64 value.
 *
 * @return a long value.
 */
qint64 OscLong::get()
{
	return mPacket->getLong(mPos);
}

bool OscLong::toBoolean()
{
	return (get() != 0);
}

QByteArray OscLong::toBytes()
{
	qint64 l = get();
	QByteArray bytes(8, 0);
	bytes.insert(0, (char*)&l, 8);
	return bytes;
}

char OscLong::toChar()
{
	qint64 l = get();
	if (l > std::numeric_limits<char>::min() && l < std::numeric_limits<char>::max())
		return (char)get();
	else
		throw CharConversionException();
}

double OscLong::toDouble()
{
	return get();
}

qint64 OscLong::toLong()
{
	return get();
}

Midi OscLong::toMIDI()
{
	return Midi(toBytes());
}

RGBA OscLong::toRGBA()
{
	return RGBA(toBytes());
}

QString OscLong::toString()
{
	return QString::number(get());
}

QDateTime OscLong::toTimetag()
{
	NtpTimestamp ntp( *(qint32*)(toBytes().constData()), *(qint32*)(toBytes().constData() + 4));
	return NtpTimestamp::toDateTime(ntp);
}

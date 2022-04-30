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
 
#include <osc/reader/types/OscDouble.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <limits>
#include <tools/NtpTimestamp.h>

OscDouble::OscDouble(ByteBuffer* packet, qint32 pos)
: OscValue('d', packet, pos)
{
	mPacket->getDouble();
}

/**
 * Returns the double value.
 *
 * @return a double value.
 */
double OscDouble::get()
{
	return mPacket->getDouble(mPos);
}

bool OscDouble::toBoolean()
{
	return (get() != 0);
}

QByteArray OscDouble::toBytes()
{
	double d = get();
	QByteArray bytes(8, 0);
	bytes.insert(0, (char*)&d, 8);
	return bytes;
}

char OscDouble::toChar()
{
	double d = get();
	if (d > std::numeric_limits<char>::min() && d < std::numeric_limits<char>::max())
		return (char)get();
	else
		throw CharConversionException();
}

double OscDouble::toDouble()
{
	return get();
}

qint64 OscDouble::toLong()
{
	return (qint64)get();
}

Midi OscDouble::toMIDI()
{
	return Midi(toBytes());
}

RGBA OscDouble::toRGBA()
{
	return RGBA(toBytes());
}

QString OscDouble::toString()
{
	return QString::number(get());
}

QDateTime OscDouble::toTimetag()
{
	NtpTimestamp ntp( *(qint32*)(toBytes().constData()), *(qint32*)(toBytes().constData() + 4));
	return NtpTimestamp::toDateTime(ntp);
}

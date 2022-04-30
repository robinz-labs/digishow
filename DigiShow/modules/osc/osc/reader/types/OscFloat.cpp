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
 
#include <osc/reader/types/OscFloat.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <limits>

OscFloat::OscFloat(ByteBuffer* packet, qint32 pos)
: OscValue('f', packet, pos)
{
	mPacket->getFloat();
}

float OscFloat::get()
{
	return mPacket->getFloat(mPos);
}

bool OscFloat::toBoolean()
{
	return (get() != 0);
}

QByteArray OscFloat::toBytes()
{
	float f = get();
	QByteArray bytes(4, 0);
	bytes.insert(0, (char*)&f, 4);
	return bytes;
}

char OscFloat::toChar()
{
	float f = get();
	if (f > std::numeric_limits<char>::min() && f < std::numeric_limits<char>::max())
		return (char)get();
	else
		throw CharConversionException();
}

double OscFloat::toDouble()
{
	return get();
}

qint64 OscFloat::toLong()
{
	return (qint64)get();
}

Midi OscFloat::toMIDI()
{
	return Midi(toBytes());
}

RGBA OscFloat::toRGBA()
{
	return RGBA(toBytes());
}

QString OscFloat::toString()
{
	return QString::number(get());
}

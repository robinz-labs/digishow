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
 
#include <osc/reader/types/OscInteger.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <limits>

OscInteger::OscInteger(ByteBuffer* packet, qint32 pos)
: OscValue('i', packet, pos)
{
	mPacket->getInt();
}

qint32 OscInteger::get()
{
	return mPacket->getInt(mPos);
}

bool OscInteger::toBoolean()
{
	return (get() != 0);
}

QByteArray OscInteger::toBytes()
{
	qint32 i = get();
	QByteArray bytes(4, 0);
	bytes.insert(0, (char*)&i, 4);
	return bytes;
}

char OscInteger::toChar()
{
	qint32 i = get();
	if (i > std::numeric_limits<char>::min() && i < std::numeric_limits<char>::max())
		return (char)get();
	else
		throw CharConversionException();
}

double OscInteger::toDouble()
{
	return get();
}

qint64 OscInteger::toLong()
{
	return get();
}

Midi OscInteger::toMIDI()
{
	return Midi(toBytes());
}

RGBA OscInteger::toRGBA()
{
	return RGBA(toBytes());
}

QString OscInteger::toString()
{
	return QString::number(get());
}


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
 
#include <osc/reader/types/OscRGBA.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>

/**
 * Build a new OscRGBA from the given location in packet.
 *
 * @param pos
 *            the RGBA color value position.
 */
OscRGBA::OscRGBA(ByteBuffer* packet, qint32 pos)
: OscValue('r', packet, pos)
{
	mPacket->getInt();
}

/**
 * Returns the RGBA color value.
 *
 * @return an qint32 value.
 */
QByteArray OscRGBA::get()
{
	qint32 i = mPacket->getInt(mPos);
	QByteArray bytes(4, 0);
	bytes.insert(0, (char*)&i, 4);
	return bytes;
}

bool OscRGBA::toBoolean()
{
	return (mPacket->getInt(mPos) != 0);
}

QByteArray OscRGBA::toBytes()
{
	return get();
}

char OscRGBA::toChar()
{
	throw CharConversionException();
}

float OscRGBA::toFloat()
{
	throw FloatConversionException();
}

double OscRGBA::toDouble()
{
	throw DoubleConversionException();
}

qint64 OscRGBA::toLong()
{
	return mPacket->getInt(mPos);
}

Midi OscRGBA::toMIDI()
{
	return Midi(get());
}

RGBA OscRGBA::toRGBA()
{
	return RGBA(get());
}

QString OscRGBA::toString()
{
	return QString::number(mPacket->getInt(mPos),16);
}

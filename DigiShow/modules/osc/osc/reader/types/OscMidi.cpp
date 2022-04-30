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
 
#include <osc/reader/types/OscMidi.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>

/**
 * Build a new OscMidi from the given location in packet.
 *
 * @param pos
 *            the MIDI message position.
 */
OscMidi::OscMidi(ByteBuffer* packet, qint32 pos)
: OscValue('m', packet, pos)
{
	mPacket->getInt();
}

/**
 * Returns the MIDI message value.
 *
 * @return an qint32 value.
 */
QByteArray OscMidi::get()
{
	QByteArray bytes(4, 0);
	mPacket->setPosition(mPos);
	mPacket->get(&bytes);
	return bytes;
}

bool OscMidi::toBoolean()
{
	return (mPacket->getInt(mPos) != 0);
}

QByteArray OscMidi::toBytes()
{
	return get();
}

char OscMidi::toChar()
{
	throw CharConversionException();
}

float OscMidi::toFloat()
{
	throw FloatConversionException();
}

double OscMidi::toDouble()
{
	throw DoubleConversionException();
}

qint64 OscMidi::toLong()
{
	return mPacket->getInt(mPos);
}

Midi OscMidi::toMIDI()
{
	return Midi(get());
}

RGBA OscMidi::toRGBA()
{
	return RGBA(get());
}

QString OscMidi::toString()
{
	return QString::number(mPacket->getInt(mPos),16);
}

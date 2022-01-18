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
 
#include <osc/reader/types/OscTags.h>
#include <osc/exceptions/BooleanConversionException.h>
#include <osc/exceptions/BytesConversionException.h>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/IntegerConversionException.h>
#include <osc/exceptions/LongConversionException.h>
#include <osc/exceptions/MidiConversionException.h>
#include <osc/exceptions/RgbaConversionException.h>
#include <osc/exceptions/SymbolConversionException.h>
#include <osc/exceptions/TimetagConversionException.h>

/**
 * Build a new OscTags from the given location in packet.
 *
 * @param pos
 *            the message type tags string position.
 */
OscTags::OscTags(ByteBuffer* packet, qint32 pos)
: OscValue('\0', packet, pos)
{
	try
	{
		get();
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Returns the OSC type tags associated to this message.
 *
 * @return a string value.
 */
QString OscTags::get()
{
	try
	{
		return OscValue::getString(mPacket, mPos);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

bool OscTags::toBoolean()
{
	throw BooleanConversionException();
}

QByteArray OscTags::toBytes()
{
	throw BytesConversionException();
}

char OscTags::toChar()
{
	throw CharConversionException();
}

double OscTags::toDouble()
{
	throw DoubleConversionException();
}

float OscTags::toFloat()
{
	throw FloatConversionException();
}

qint32 OscTags::toInteger()
{
	throw IntegerConversionException();
}

qint64 OscTags::toLong()
{
	throw LongConversionException();
}

Midi OscTags::toMIDI()
{
	throw MidiConversionException();
}

RGBA OscTags::toRGBA()
{
	throw RgbaConversionException();
}

QString OscTags::toString()
{
	return get();
}

QString OscTags::toSymbol()
{
	throw SymbolConversionException();
}

QDateTime OscTags::toTimetag()
{
	throw TimetagConversionException();
}

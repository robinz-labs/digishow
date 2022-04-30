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
 
#include <osc/reader/types/OscTimetag.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>
#include <osc/exceptions/IntegerConversionException.h>
#include <osc/exceptions/LongConversionException.h>
#include <osc/exceptions/StringConversionException.h>
#include <limits>
#include <tools/NtpTimestamp.h>
#include <QtEndian>

/**
 * Build a new OscTimetag from the given location in packet.
 *
 * @param pos
 *            the time tag value position.
 */
OscTimetag::OscTimetag(ByteBuffer* packet, qint32 pos)
: OscValue('t', packet, pos)
{
	mPacket->getLong();
}

/**
 * Returns the OSC Timetag value.
 *
 * @return a long value.
 */
NtpTimestamp OscTimetag::get()
{
	NtpTimestamp ntp( mPacket->getInt(mPos), mPacket->getInt(mPos + 4) );
	return ntp;
}

bool OscTimetag::toBoolean()
{
	NtpTimestamp ntp( qToBigEndian(0), qToBigEndian(1) );
	return (get() != ntp);
}

QByteArray OscTimetag::toBytes()
{
	NtpTimestamp ntp = get();
	QByteArray bytes(8, 0);
	quint32 secs = ntp.rawSeconds();
	quint32 fracs = ntp.rawFraction();
	bytes.insert(0, (char*)&secs, 4);
	bytes.insert(4, (char*)&fracs, 4);
	return bytes;
}

char OscTimetag::toChar()
{
	throw CharConversionException();
}

double OscTimetag::toDouble()
{
	throw DoubleConversionException();
}

float OscTimetag::toFloat()
{
	throw FloatConversionException();
}

qint32 OscTimetag::toInteger()
{
	throw IntegerConversionException();
}

qint64 OscTimetag::toLong()
{
	throw LongConversionException();
}

Midi OscTimetag::toMIDI()
{
	NtpTimestamp ntp = get();
	QByteArray bytes(4, 0);
	quint32 secs = ntp.seconds();
	bytes.insert(0, (char*)&secs, 4);
	return Midi(bytes);
}

RGBA OscTimetag::toRGBA()
{
	NtpTimestamp ntp = get();
	QByteArray bytes(4, 0);
	quint32 secs = ntp.seconds();
	bytes.insert(0, (char*)&secs, 4);
	return RGBA(bytes);
}

QString OscTimetag::toString()
{
	throw StringConversionException();
}

QDateTime OscTimetag::toTimetag()
{
	return NtpTimestamp::toDateTime(get());
}

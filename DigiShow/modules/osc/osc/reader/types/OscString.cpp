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
 
#include <osc/reader/types/OscString.h>
#include <QtCore/QHash>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>
#include <osc/exceptions/LongConversionException.h>
#include <osc/exceptions/IntegerConversionException.h>

/**
 * Build a new OscString from the given location in packet.
 *
 * @param pos
 *            the string position.
 */
OscString::OscString(ByteBuffer* packet, qint32 pos)
: OscValue('s', packet, pos)
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
 * Returns the string value.
 *
 * @return a string value.
 */
QByteArray OscString::get()
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

bool OscString::toBoolean()
{
	return (get().length() != 0);
}

QByteArray OscString::toBytes()
{
	return get();
}

char OscString::toChar()
{
	const QByteArray& value = get();
	if (value.length() > 1)
		throw CharConversionException();
	else
		return value.at(0);
}

float OscString::toFloat()
{
	bool ok;
	float f = get().toFloat(&ok);
	if (ok)
		return f;
	else
		throw FloatConversionException();
}

double OscString::toDouble()
{
	bool ok;
	double d = get().toDouble(&ok);
	if (ok)
		return d;
	else
		throw DoubleConversionException();
}

qint32 OscString::toInteger()
{
	bool ok;
	qint32 i = get().toInt(&ok);
	if (ok)
		return i;
	else
		throw IntegerConversionException();
}

qint64 OscString::toLong()
{
	bool ok;
	qint64 h = get().toLongLong(&ok);
	if (ok)
		return h;
	else
		throw LongConversionException();
}

QString OscString::toString()
{
	return get();
}

QString OscString::toSymbol()
{
	return toString();
}

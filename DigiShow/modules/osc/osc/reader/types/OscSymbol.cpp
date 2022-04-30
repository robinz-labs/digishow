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
 
#include <osc/reader/types/OscSymbol.h>
#include <QtCore/QHash>
#include <osc/exceptions/CharConversionException.h>
#include <osc/exceptions/IntegerConversionException.h>
#include <osc/exceptions/FloatConversionException.h>
#include <osc/exceptions/DoubleConversionException.h>
#include <osc/exceptions/LongConversionException.h>

/**
 * Build a new OscSymbol from the given location in packet.
 *
 * @param pos
 *            the Symbol string value position.
 */
OscSymbol::OscSymbol(ByteBuffer* packet, qint32 pos)
: OscValue('S', packet, pos)
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
 * Returns the Symbol value.
 *
 * @return a string value.
 */
QByteArray OscSymbol::get()
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

bool OscSymbol::toBoolean()
{
	return (get().length() != 0);
}

QByteArray OscSymbol::toBytes()
{
	return get();
}

char OscSymbol::toChar()
{
	const QByteArray& value = get();
	if (value.length() > 1)
		throw CharConversionException();
	else
		return value.at(0);
}

float OscSymbol::toFloat()
{
	bool ok;
	float f = get().toFloat(&ok);
	if (ok)
		return f;
	else
		throw FloatConversionException();
}

double OscSymbol::toDouble()
{
	bool ok;
	double d = get().toDouble(&ok);
	if (ok)
		return d;
	else
		throw DoubleConversionException();
}

qint32 OscSymbol::toInteger()
{
	bool ok;
	qint32 i = get().toInt(&ok);
	if (ok)
		return i;
	else
		throw IntegerConversionException();
}

qint64 OscSymbol::toLong()
{
	bool ok;
	qint64 h = get().toLongLong(&ok);
	if (ok)
		return h;
	else
		throw LongConversionException();
}

QString OscSymbol::toString()
{
	return toSymbol();
}

QString OscSymbol::toSymbol()
{
	return get();
}

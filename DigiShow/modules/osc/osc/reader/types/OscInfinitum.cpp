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
 
#include <osc/reader/types/OscInfinitum.h>
#include <limits>

/**
 * Build a new OscInfinitum from the given location in packet.
 *
 * @param pos
 *            the Infinitum tag position.
 */
OscInfinitum::OscInfinitum(ByteBuffer* packet, qint32 pos)
: OscValue('I', packet, pos)
{
}

bool OscInfinitum::isInfinitum()
{
	return true;
}

bool OscInfinitum::toBoolean()
{
	return true;
}

QByteArray OscInfinitum::toBytes()
{
	return QByteArray(1, std::numeric_limits<unsigned char>::infinity());
}

char OscInfinitum::toChar()
{
	return std::numeric_limits<char>::infinity();
}

double OscInfinitum::toDouble()
{
	return std::numeric_limits<double>::infinity();
}

float OscInfinitum::toFloat()
{
	return std::numeric_limits<float>::infinity();
}

qint32 OscInfinitum::toInteger()
{
	return std::numeric_limits<qint32>::infinity();
}

qint64 OscInfinitum::toLong()
{
	return std::numeric_limits<qint64>::infinity();
}

QString OscInfinitum::toString()
{
	return "INF";
}

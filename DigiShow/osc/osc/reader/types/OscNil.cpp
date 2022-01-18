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
 
#include <osc/reader/types/OscNil.h>

/**
 * Build a new OscNil from the given location in packet.
 *
 * @param pos
 *            the Nil tag position.
 */
OscNil::OscNil(ByteBuffer* packet, qint32 pos)
: OscValue('N', packet, pos)
{
}

bool OscNil::isNil()
{
	return true;
}

char OscNil::toChar()
{
	return 0;
}

QByteArray OscNil::toBytes()
{
	return QByteArray(1, 0);
}

double OscNil::toDouble()
{
	return 0;
}

qint64 OscNil::toLong()
{
	return 0;
}

QString OscNil::toString()
{
	return "NIL";
}

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
 
#include <osc/reader/types/OscChar.h>
#include <tools/ByteBuffer.h>

/**
 * Build a new OscChar from the given location in packet.
 *
 * @param pos
 *            the char value position.
 */
OscChar::OscChar(ByteBuffer* packet, qint32 pos)
: OscValue('c', packet, pos)
{
	mPacket->getInt();
}

/**
 * Returns the char value.
 *
 * @return a char value.
 */
qint32 OscChar::get()
{
	return mPacket->getInt(mPos);
}

bool OscChar::toBoolean()
{
	return (get() != 0);
}

QByteArray OscChar::toBytes()
{
	return QByteArray(1, (char)get());
}

char OscChar::toChar()
{
	return get();
}

double OscChar::toDouble()
{
	return get();
}

qint64 OscChar::toLong()
{
	return get();
}

QString OscChar::toString()
{
	return QChar(get());
}

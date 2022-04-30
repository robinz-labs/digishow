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
 
#ifndef OSC_VALUE_H_
#define OSC_VALUE_H_

#include <osc/OscAPI.h>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <osc/reader/types/Midi.h>
#include <osc/reader/types/RGBA.h>
#include <QDateTime>

class ByteBuffer;

/**
 * Abstract class representing an OSC value. This class is derivated to
 * manage all OSC common types.
 */
class OSC_API OscValue
{
public:

	virtual char getTag();
	virtual bool isArrayBegin();
	virtual bool isArrayEnd();
	virtual bool isFalse();
	virtual bool isTrue();
	virtual bool isNil();
	virtual bool isInfinitum();
	virtual bool toBoolean();
	virtual QByteArray toBytes() = 0;
	virtual char toChar();
	virtual float toFloat();
	virtual double toDouble() = 0;
	virtual qint32 toInteger();
	virtual qint64 toLong() = 0;
	virtual QString toString();
	virtual QString toSymbol();
	virtual Midi toMIDI();
	virtual RGBA toRGBA();
	virtual QDateTime toTimetag();

	virtual ~OscValue() {};

protected:

	OscValue(char typeTag, ByteBuffer* packet, qint32 pos);


	/**
	 * Returns the position of the next non-null character in the packet.
	 *
	 * @return the position.
	 */
	static qint32 getLastStringIdx(ByteBuffer* packet);

	/**
	 * Returns the string located at the passed position in the packet buffer.
	 *
	 * @param pos
	 *            The position were the string starts
	 * @return The read string.
	 */
	static QByteArray getString(ByteBuffer* packet, qint32 pos);

	/** The buffer to read to. */
	ByteBuffer* mPacket;

	/** The Osc value type tag. */
	char mTag;

	qint32 mPos;
};

#endif // OSC_VALUE_H_

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
 
#ifndef OSC_MESSAGE_COMPOSER_H_
#define OSC_MESSAGE_COMPOSER_H_

#include <osc/composer/OscContentComposer.h>
#include <QDateTime>
#include <osc/reader/types/OscTimetag.h>

class OSC_API OscMessageComposer : public OscContentComposer
{
public:
	/**
	 * Build a new OSC message
	 *
	 * @param address
	 *            The address pattern to set to the message.
	 */
	OscMessageComposer(const QString& address);

    /**
     * Build a new OSC message
     *
     * @param address
     *            The address pattern to set to the message.
     */
	OscMessageComposer(const char* str);

	/**
	 *
	 */
	virtual ~OscMessageComposer();

	/**
	 * @brief init
	 * @param address
	 */
	void init(const QString& address);

	/**
	 * Gets the complete OSC message for the current message being composed.
	 *
	 * @return the current OscMessage data.
	 */
	virtual QByteArray* getBytes(/*OscVersion::Version aVersion = OscVersion::OSC_10*/);

	/**
	 * Indicates the beginning of an array in the OscMessage being composed.
	 */
	void pushArrayBegin();

	/**
	 * Indicates the end of an array in the OscMessage being composed.
	 *
	 * Does nothing if no array has been started.
	 */
	void pushArrayEnd();

	/**
	 * Pushes the whole passed bytes array into this message.
	 *
	 * @param src
	 *            The array from which bytes are to be read
	 */
	void pushBlob(const QByteArray& src);

	/**
	 * Pushes the whole, or part of, bytes array into this message.
	 *
	 * @param src
	 *            The array from which bytes are to be read
	 * @param offset
	 *            The offset within the array of the first byte to be read; must
	 *            be non-negative and no larger than array.length
	 * @param length
	 *            The number of bytes to be read from the given array; must be
	 *            non-negative and no larger than array.length - offset
	 */
	void pushBlob(QByteArray* src, qint32 offset, qint32 length);

	/**
	 * Appends a <i>True</i> or <i>False</i> flag to OscMessage being composed,
	 * respectively to the passed boolean value.
	 *
	 * @param b
	 *            The boolean value to add to the message
	 */
	void pushBool(bool b);

	/**
	 * Appends a <i>ASCII character</i> value to OscMessage being composed.
	 *
	 * @param c
	 *            The ASCII character value to add to the message.
	 */
	void pushChar(char c);

	/**
	 * Appends a <i>double</i> value to OscMessage being composed.
	 *
	 * @param d
	 *            The double value to add to the message.
	 */
	void pushDouble(double d);

	/**
	 * Appends a <i>False</i> flag to OscMessage being composed.
	 */
	void pushFalse();

	/**
	 * Appends a <i>32-bit float</i> value to OscMessage being composed.
	 *
	 * @param f
	 *            The float value to add to the message.
	 */
	void pushFloat(float f);

	/**
	 * Appends an <i>Infinitum</i> flag to OscMessage being composed.
	 */
	void pushInfinitum();

	/**
	 * Appends a <i>32-bit integer</i> value to OscMessage being composed.
	 *
	 * @param i
	 *            The int32 value to add to the message.
	 */
	void pushInt32(qint32 i);

	/**
	 * Appends a <i>64-bit integer</i> value to OscMessage being composed.
	 *
	 * @param l
	 *            The long value to add to the message.
	 */
	void pushLong(qint64 l);

	/**
	 * Appends a <i>4-byte MIDI message</i> value to OscMessage being composed.
	 *
	 * @param m
	 *            The MIDI message value to add to the message.
	 */
	void pushMidi(char port, char status, char data1, char data2);

	/**
	 * Appends an <i>Nil</i> flag to OscMessage being composed.
	 */
	void pushNil();

	/**
	 * Appends a <i>32-bit RGBA color</i> value to OscMessage being composed.
	 *
	 * @param r
	 *            The RGBA color value to add to the message.
	 */
	void pushRGBA(qint32 r);

	/**
	 * Appends a <i>String</i> to OscMessage being composed.
	 *
	 * @param s
	 *            The string to add to the message.
	 */
	void pushString(const QString& s);

	/**
	 * Appends a <i>Symbol</i> to OscMessage being composed.
	 *
	 * @param S
	 *            The symbol to add to the message.
	 */
	void pushSymbol(QString& S);

	/**
	 * Appends a <i>Time Tag</i> to OscMessage being composed.
	 *
	 * @param t
	 *            The time tag value as a long value to add to the message.
	 */
	void pushTimeTag(QDateTime& t );

	/**
	 * Appends a <i>True</i> flag to OscMessage being composed.
	 */
	void pushTrue();

protected:

	/*
	 * @see com.cocon.osc.OscComposer#fillByteBuffer(java.nio.ByteBuffer)
	 */
	virtual void fillByteBuffer(ByteBuffer* _byteBuffer);

	/**
	 * @see com.cocon.osc.OscComposer#getSize()
	 */
	virtual qint32 computeSize();

private:
	OscMessageComposer() {};
	qint32 mArrayLevel;
	QByteArray* mDataBuffer;
	static qint32 mDataBufferSize;
	ByteBuffer* mDataByteBuffer;
};

#endif // OSC_MESSAGE_COMPOSER_H_

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
 
#include <osc/composer/OscMessageComposer.h>
#include <osc/OscVersion.h>
#include <tools/ByteBuffer.h>

#include <tools/ByteOrder.h>
#include <cstring>
#include <new>

qint32 OscMessageComposer::mDataBufferSize = 128;

OscMessageComposer::OscMessageComposer(const QString& address)
: OscContentComposer(), mArrayLevel(0)
{
    init(address);
}

OscMessageComposer::OscMessageComposer(const char* str)
: OscContentComposer(), mArrayLevel(0)
{
    QString s(str);
    init(s);
}

void
OscMessageComposer::init(const QString& address)
{
    try
    {
        mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, address.length() + 1);
        QByteArray addressByteArray(address.toLocal8Bit());
        mHeaderByteBuffer->put(&addressByteArray);
        mHeaderByteBuffer->put((char)0);        
        fillAlignment(mHeaderByteBuffer);

        mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
        mHeaderByteBuffer->put((char) ',');
    }
    catch (const QException& e)
    {
		throw e;
    }

    mDataBuffer = new QByteArray(mDataBufferSize, 0);
    mDataByteBuffer = ByteBuffer::wrap(mDataBuffer, true);
    mDataByteBuffer->setOrder(ByteOrder::BIG_ENDIAN_DATA);
}

OscMessageComposer::~OscMessageComposer()
{
	// No Need to delete since buffer is owned by ByteBuffer
	// delete mDataBuffer;

	delete mDataByteBuffer;
}

/**
 * Gets the complete OSC message for the current message being composed.
 *
 * @return the current OscMessage data.
 */
QByteArray* OscMessageComposer::getBytes(/*OscVersion::Version aVersion*/)
{
	// Be sure to end all opened arrays
	while (mArrayLevel != 0)
	{
		pushArrayEnd();
	}

	try
	{
		fillAlignment(mHeaderByteBuffer);
	}
	catch (const QException& e)
	{
		throw e;
	}

	qint32 size = computeSize();

	qint32 offset = 0;
	QByteArray* tempData;
	/*if (aVersion == OscVersion::OSC_10) {
		offset = 4;
		tempData = new QByteArray(size + 4, 0);
		ByteBuffer* bb = ByteBuffer::wrap(tempData);
		bb->putInt(size);
		delete bb;
	}
	else {*/
		tempData = new QByteArray(size, 0);
	/*}*/

	memcpy(offset + tempData->data(), mHeaderByteBuffer->getByteArray()->data(), mHeaderByteBuffer->getPosition());
	memcpy(offset + tempData->data() + mHeaderByteBuffer->getPosition(), mDataByteBuffer->getByteArray()->data(), mDataByteBuffer->getPosition());

	return tempData;
}

qint32 OscMessageComposer::computeSize()
{
	return OscContentComposer::computeSize() + mDataByteBuffer->getPosition();
}

/**
 * Indicates the beginning of an array in the OscMessage being composed.
 */
void OscMessageComposer::pushArrayBegin()
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) '[');
		mArrayLevel++;
	}
	catch (const QException& e)
	{
		throw e;
	}
}
/**
 * Indicates the end of an array in the OscMessage being composed.
 *
 * Does nothing if no array has been started.
 */
void OscMessageComposer::pushArrayEnd()
{
	if (mArrayLevel > 0)
	{
		try
		{
			mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
			mHeaderByteBuffer->put((char) ']');
			mArrayLevel--;
		}
		catch (const QException& e)
		{
			throw e;
		}
	}
}

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
void OscMessageComposer::pushBlob(QByteArray* src, qint32 offset, qint32 length)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'b');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->putInt(length);

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, length);
		mDataByteBuffer->put(src, offset, length);

		fillAlignment(mDataByteBuffer);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

void
OscMessageComposer::pushBlob(const QByteArray& src)
{
	pushBlob((QByteArray*) &src, 0, src.size());
}

/**
 * Appends a <i>True</i> or <i>False</i> flag to OscMessage being composed,
 * respectively to the passed boolean value.
 *
 * @param b
 *            The boolean value to add to the message
 */
void OscMessageComposer::pushBool(bool b)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char)(b ? 'T' : 'F'));
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>ASCII character</i> value to OscMessage being composed.
 *
 * @param c
 *            The ASCII character value to add to the message.
 */
void OscMessageComposer::pushChar(char c)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'c');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->putInt(c);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>double</i> value to OscMessage being composed.
 *
 * @param d
 *            The double value to add to the message.
 */
void OscMessageComposer::pushDouble(double d)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'd');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 8);
		mDataByteBuffer->putDouble(d);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>False</i> flag to OscMessage being composed.
 */
void OscMessageComposer::pushFalse()
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'F');
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>32-bit float</i> value to OscMessage being composed.
 *
 * @param f
 *            The float value to add to the message.
 */
void OscMessageComposer::pushFloat(float f)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'f');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->putFloat(f);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends an <i>Infinitum</i> flag to OscMessage being composed.
 */
void OscMessageComposer::pushInfinitum()
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'I');
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>32-bit integer</i> value to OscMessage being composed.
 *
 * @param i
 *            The int32 value to add to the message.
 */
void OscMessageComposer::pushInt32(qint32 i)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'i');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->putInt(i);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>64-bit integer</i> value to OscMessage being composed.
 *
 * @param l
 *            The long value to add to the message.
 */
void OscMessageComposer::pushLong(qint64 l)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'h');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 8);
		mDataByteBuffer->putLong(l);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>4-byte MIDI message</i> value to OscMessage being composed.
 *
 * @param m
 *            The MIDI message value to add to the message.
 */
void OscMessageComposer::pushMidi(char port, char status, char data1, char data2)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'm');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->put(port);
		mDataByteBuffer->put(status);
		mDataByteBuffer->put(data1);
		mDataByteBuffer->put(data2);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends an <i>Nil</i> flag to OscMessage being composed.
 */
void OscMessageComposer::pushNil()
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'N');
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>32-bit RGBA color</i> value to OscMessage being composed.
 *
 * @param r
 *            The RGBA color value to add to the message.
 */
void OscMessageComposer::pushRGBA(qint32 r)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'r');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 4);
		mDataByteBuffer->putInt(r);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>String</i> to OscMessage being composed.
 *
 * @param s
 *            The string to add to the message.
 */
void OscMessageComposer::pushString(const QString& s)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 's');

		QByteArray sArray = s.toUtf8();
		mDataByteBuffer = checkBufferSize(mDataByteBuffer, sArray.length()+1);
		mDataByteBuffer->put(&sArray);
		mDataByteBuffer->put((char) '\0');

		fillAlignment(mDataByteBuffer);
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>Symbol</i> to OscMessage being composed.
 *
 * @param S
 *            The symbol to add to the message.
 */
void OscMessageComposer::pushSymbol(QString& S)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'S');

		QByteArray sArray = S.toUtf8();
		mDataByteBuffer = checkBufferSize(mDataByteBuffer, sArray.length()+1);
		mDataByteBuffer->put(&sArray);
		mDataByteBuffer->put((char) '\0');

		fillAlignment(mDataByteBuffer);
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>Time Tag</i> to OscMessage being composed.
 *
 * @param t
 *            The time tag value as a long value to add to the message.
 */
void OscMessageComposer::pushTimeTag(QDateTime& t)
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 't');

		mDataByteBuffer = checkBufferSize(mDataByteBuffer, 8);

		NtpTimestamp ntp = NtpTimestamp::fromDateTime(t);
		QByteArray seconds(4,0);
		qint32 secs = ntp.rawSeconds();
		seconds.insert(0, (char*)&secs, 4);
		mDataByteBuffer->put( &seconds, 0, 4);

		QByteArray fraction(4,0);
		qint32 frac = ntp.rawFraction();
		fraction.insert(0, (char*)&frac, 4 );
		mDataByteBuffer->put( &fraction, 0, 4 );

	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/**
 * Appends a <i>True</i> flag to OscMessage being composed.
 */
void OscMessageComposer::pushTrue()
{
	try
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char) 'T');
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/*
 * @see com.cocon.osc.OscComposer#fillByteBuffer(java.nio.ByteBuffer)
 */
void OscMessageComposer::fillByteBuffer(ByteBuffer* _byteBuffer)
{
	try
	{
		OscContentComposer::fillByteBuffer(_byteBuffer);
		_byteBuffer->put(mDataByteBuffer->getByteArray(), 0, mDataByteBuffer->getPosition());
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

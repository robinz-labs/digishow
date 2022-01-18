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

#include <tools/ByteBuffer.h>

#include <QtCore/QString>

ByteBuffer::ByteBuffer(QByteArray* src, bool owner)
: mArray(src)
, mOrder(ByteOrder::BIG_ENDIAN_DATA)
, mLimit(src->size())
, mPosition(0)
, mOwner(owner)
{
}

ByteBuffer::~ByteBuffer()
{
	if (mOwner == true)
		delete mArray;
}

/**
 * Creates a byte buffer based on a newly allocated byte array.
 *
 * @param capacity
 *            the capacity of the new buffer
 * @return the created byte buffer.
 * @throws IllegalArgumentException
 *             if {@code capacity < 0}.
 */
ByteBuffer* ByteBuffer::allocate(qint32 capacity)
{
	if (capacity < 0)
	{
		throw IllegalArgumentException();
	}
	return ByteBuffer::wrap(new QByteArray(capacity, 0), true);
}

/**
 * Creates a new byte buffer by wrapping the given byte array.
 * <p>
 * Calling this method has the same effect as
 * {@code wrap(array, 0, array.length)}.
 *
 * @param array
 *            the byte array which the new buffer will be based on
 * @return the created byte buffer.
 */
ByteBuffer* ByteBuffer::wrap(QByteArray* array, bool owner)
{
	return new ByteBuffer(array, owner);
}

/**
 * Creates a new byte buffer by wrapping the given byte array.
 * <p>
 * The new buffer's position will be {@code start}, limit will be
 * {@code start + len}, capacity will be the length of the array.
 *
 * @param array
 *            the byte array which the new buffer will be based on.
 * @param start
 *            the start index, must not be negative and not greater than
 *            {@code array.length}.
 * @param len
 *            the length, must not be negative and not greater than
 *            {@code array.length - start}.
 * @return the created byte buffer.
 * @exception IndexOutOfBoundsException
 *                if either {@code start} or {@code len} is invalid.
 */

ByteBuffer* ByteBuffer::wrap(QByteArray* array, qint32 start, qint32 len)
{
	qint32 length = array->size();
	if ((start < 0) || (len < 0) || (start + len > length))
		throw IndexOutOfBoundsException();

	ByteBuffer* buf = new ByteBuffer(array, false);
	buf->mPosition = start;
	buf->mLimit = start + len;

	return buf;
}

/**
 * Returns the byte array which this buffer is based on, if there is one.
 *
 * @return the byte array which this buffer is based on.
 * @exception UnsupportedOperationException
 *                if this buffer is not based on an array.
 */
QByteArray* ByteBuffer::getByteArray() const
{
	return mArray;
}

/**
 * Returns this buffer's position. </p>
 *
 * @return  The position of this buffer
 */
qint32 ByteBuffer::getPosition()
{
	return mPosition;
}

/**
 *
 */
qint32 ByteBuffer::getCapacity()
{
	return mLimit;
}

/**
 * Sets this buffer's position.  If the mark is defined and larger than the
 * new position then it is discarded. </p>
 *
 * @param  newPosition
 *         The new position value; must be non-negative
 *         and no larger than the current limit
 *
 * @return  This buffer
 *
 * @throws  IllegalArgumentException
 *          If the preconditions on <tt>newPosition</tt> do not hold
 */
ByteBuffer* ByteBuffer::setPosition(qint32 newPosition)
{
	if ((newPosition > mLimit) || (newPosition < 0))
		throw IllegalArgumentException();
	mPosition = newPosition;
	return this;
}

/**
 * Compares the remaining bytes of this buffer to another byte buffer's
 * remaining bytes.
 *
 * @param otherBuffer
 *            another byte buffer.
 * @return a negative value if this is less than {@code other}; 0 if this
 *         equals to {@code other}; a positive value if this is greater
 *         than {@code other}.
 * @exception ClassCastException
 *                if {@code other} is not a byte buffer.
 */
qint32 ByteBuffer::compareTo(ByteBuffer* otherBuffer)
{
	qint32 compareRemaining = (getRemaining() < otherBuffer->getRemaining()) ? getRemaining() : otherBuffer->getRemaining();
	qint32 thisPos = mPosition;
	qint32 otherPos = otherBuffer->mPosition;
	char thisByte, otherByte;
	while (compareRemaining > 0)
	{
		thisByte = get(thisPos);
		otherByte = otherBuffer->get(otherPos);
		if (thisByte != otherByte)
		{
			return thisByte < otherByte ? -1 : 1;
		}
		thisPos++;
		otherPos++;
		compareRemaining--;
	}
	return getRemaining() - otherBuffer->getRemaining();
}

/**
 * Returns the byte at the current position and increases the position by 1.
 *
 * @return the byte at the current position.
 * @exception BufferUnderflowException
 *                if the position is equal or greater than limit.
 */
char ByteBuffer::get()
{
	return (mArray->data()[mPosition++]);
}

/**
 * Reads bytes from the current position into the specified byte array and
 * increases the position by the number of bytes read.
 * <p>
 * Calling this method has the same effect as
 * {@code get(dest, 0, dest.length)}.
 *
 * @param dest
 *            the destination byte array.
 * @return this buffer.
 * @exception BufferUnderflowException
 *                if {@code dest.length} is greater than {@code remaining()}.
 */
ByteBuffer* ByteBuffer::get(QByteArray* dest)
{
	try
	{
		return get(dest, 0, dest->length());
	}
	catch (const IndexOutOfBoundsException& e)
	{
		throw e;
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/**
 * Reads bytes from the current position into the specified byte array,
 * starting at the specified offset, and increases the position by the
 * number of bytes read.
 *
 * @param dest
 *            the target byte array.
 * @param off
 *            the offset of the byte array, must not be negative and
 *            not greater than {@code dest.length}.
 * @param len
 *            the number of bytes to read, must not be negative and not
 *            greater than {@code dest.length - off}
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if either {@code off} or {@code len} is invalid.
 * @exception BufferUnderflowException
 *                if {@code len} is greater than {@code remaining()}.
 */
ByteBuffer* ByteBuffer::get(QByteArray* dest, qint32 off, qint32 len)
{
	qint32 length = dest->length();
	if ((off < 0) || (len < 0) || (off + len > length))
		throw IndexOutOfBoundsException();

	if (len > getRemaining())
		throw BufferUnderflowException();

	dest->replace(off, len, mArray->data() + mPosition, len);
	mPosition += len;
	return this;
}

/**
 * Returns the byte at the specified index and does not change the position.
 *
 * @param index
 *            the index, must not be negative and less than limit.
 * @return the byte at the specified index.
 * @exception IndexOutOfBoundsException
 *                if index is invalid.
 */
char ByteBuffer::get(qint32 index)
{
	char* s = mArray->data();
	return s[index];
}

/**
 * Returns the char at the current position and increases the position by 2.
 * <p>
 * The 2 bytes starting at the current position are composed into a char
 * according to the current byte order and returned.
 *
 * @return the char at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 2}.
 */
char ByteBuffer::getChar()
{
	return getChar(mPosition++);
}

/**
 * Returns the char at the specified index.
 * <p>
 * The 2 bytes starting from the specified index are composed into a char
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 2}.
 * @return the char at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
char ByteBuffer::getChar(qint32 index)
{
	return get(index);
}

/**
 * Returns the double at the current position and increases the position by
 * 8.
 * <p>
 * The 8 bytes starting from the current position are composed into a double
 * according to the current byte order and returned.
 *
 * @return the double at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 8}.
 */
double ByteBuffer::getDouble()
{
	qint64 l = getLong();
    qint64* _l = &l;
    double* _d = (double*)_l;
    return *_d;
}

/**
 * Returns the double at the specified index.
 * <p>
 * The 8 bytes starting at the specified index are composed into a double
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 8}.
 * @return the double at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
double ByteBuffer::getDouble(qint32 index)
{
	qint64 l = getLong(index);
    qint64* _l = &l;
    double* _d = (double*)_l;
    return *_d;
}

/**
 * Returns the float at the current position and increases the position by
 * 4.
 * <p>
 * The 4 bytes starting at the current position are composed into a float
 * according to the current byte order and returned.
 *
 * @return the float at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 4}.
 */
float ByteBuffer::getFloat()
{
	qint32 i = getInt();
    qint32* _i = &i;
    float* _f = (float*)_i;
    return *_f;
}

/**
 * Returns the float at the specified index.
 * <p>
 * The 4 bytes starting at the specified index are composed into a float
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 4}.
 * @return the float at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
float ByteBuffer::getFloat(qint32 index)
{
	qint32 i = getInt(index);
    qint32* _i = &i;
    float* _f = (float*)_i;
    return *_f;
}

/**
 * Returns the qint32 at the current position and increases the position by 4.
 * <p>
 * The 4 bytes starting at the current position are composed into a qint32
 * according to the current byte order and returned.
 *
 * @return the qint32 at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 4}.
 */
qint32 ByteBuffer::getInt()
{
	qint32 pos = mPosition;
	mPosition += 4;
	return getInt(pos);
}

/**
 * Returns the qint32 at the specified index.
 * <p>
 * The 4 bytes starting at the specified index are composed into a qint32
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 4}.
 * @return the qint32 at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
qint32 ByteBuffer::getInt(qint32 index)
{
	qint32 data = *(qint32*)(mArray->data() + index);
	if (mOrder == ByteOrder::endianness())
		return data;
	else
		return ((((data)& 0xff000000) >> 24) | (((data)& 0x00ff0000) >> 8) | (((data)& 0x0000ff00) << 8)
		| (((data)& 0x000000ff) << 24));
}

/**
 * Returns the long at the current position and increases the position by 8.
 * <p>
 * The 8 bytes starting at the current position are composed into a long
 * according to the current byte order and returned.
 *
 * @return the long at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 8}.
 */
qint64 ByteBuffer::getLong()
{
	qint32 pos = mPosition;
	mPosition += 8;
	return getLong(pos);
}

/**
 * Returns the long at the specified index.
 * <p>
 * The 8 bytes starting at the specified index are composed into a long
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 8}.
 * @return the long at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
qint64 ByteBuffer::getLong(qint32 index)
{
	qint64 data = *(qint64*)(mArray->data() + index);
	if (mOrder == ByteOrder::endianness())
		return data;
	else
		return ((((data)& 0xff00000000000000ull) >> 56) | (((data)& 0x00ff000000000000ull) >> 40)
		| (((data)& 0x0000ff0000000000ull) >> 24) | (((data)& 0x000000ff00000000ull) >> 8)
		| (((data)& 0x00000000ff000000ull) << 8) | (((data)& 0x0000000000ff0000ull) << 24)
		| (((data)& 0x000000000000ff00ull) << 40) | (((data)& 0x00000000000000ffull) << 56));
}

/**
 * Returns the short at the current position and increases the position by 2.
 * <p>
 * The 2 bytes starting at the current position are composed into a short
 * according to the current byte order and returned.
 *
 * @return the short at the current position.
 * @exception BufferUnderflowException
 *                if the position is greater than {@code limit - 2}.
 */
short ByteBuffer::getShort()
{
	qint32 pos = mPosition;
	mPosition += 2;
	return getShort(pos);
}

/**
 * Returns the short at the specified index.
 * <p>
 * The 2 bytes starting at the specified index are composed into a short
 * according to the current byte order and returned. The position is not
 * changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 2}.
 * @return the short at the specified index.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
short ByteBuffer::getShort(qint32 index)
{
	short data = *(short*)(mArray->data() + index);
	if (mOrder == ByteOrder::endianness())
		return data;
	else
		return ((((data) >> 8) & 0xff) | (((data)& 0xff) << 8));
}

/**
 * Calculates this buffer's hash code from the remaining chars. The
 * position, limit, capacity and mark don't affect the hash code.
 *
 * @return the hash code calculated from the remaining bytes.
 */
qint32 ByteBuffer::hashCode()
{
	qint32 myPosition = mPosition;
	qint32 hash = 0;
	while (myPosition < mLimit)
	{
		hash = hash + get(myPosition++);
	}
	return hash;
}

/**
 * Returns the byte order used by this buffer when converting bytes from/to
 * other primitive types.
 * <p>
 * The default byte order of byte buffer is always
 * {@link ByteOrder#BIG_ENDIAN BIG_ENDIAN}
 *
 * @return the byte order used by this buffer when converting bytes from/to
 *         other primitive types.
 */
ByteOrder::Endianness ByteBuffer::getOrder()
{
	return mOrder;
}

/**
 * Sets the byte order of this buffer.
 *
 * @param byteOrder
 *            the byte order to set. If {@code null} then the order
 *            will be {@link ByteOrder#LITTLE_ENDIAN LITTLE_ENDIAN}.
 * @return this buffer.
 * @see ByteOrder
 */
ByteBuffer* ByteBuffer::setOrder(ByteOrder::Endianness byteOrder)
{
	mOrder = byteOrder;
	return this;
}

/**
 * Writes the given byte to the current position and increases the position
 * by 1.
 *
 * @param b
 *            the byte to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is equal or greater than limit.
 */
ByteBuffer* ByteBuffer::put(char b)
{
	mArray->data()[mPosition++] = b;
	return this;
}

/**
 * Writes bytes in the given byte array to the current position and
 * increases the position by the number of bytes written.
 * <p>
 * Calling this method has the same effect as
 * {@code put(src, 0, src.length)}.
 *
 * @param src
 *            the source byte array.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if {@code remaining()} is less than {@code src.length}.
 */
ByteBuffer* ByteBuffer::put(QByteArray* src)
{
	try
	{
		return put(src, 0, src->length());
	}
	catch (const IndexOutOfBoundsException& e)
	{
		throw e;
	}
	catch (const BufferUnderflowException& e)
	{
		throw e;
	}
}

/**
 * Writes bytes in the given byte array, starting from the specified offset,
 * to the current position and increases the position by the number of bytes
 * written.
 *
 * @param src
 *            the source byte array.
 * @param off
 *            the offset of byte array, must not be negative and not greater
 *            than {@code src.length}.
 * @param len
 *            the number of bytes to write, must not be negative and not
 *            greater than {@code src.length - off}.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if {@code remaining()} is less than {@code len}.
 * @exception IndexOutOfBoundsException
 *                if either {@code off} or {@code len} is invalid.
 */
ByteBuffer* ByteBuffer::put(QByteArray* src, qint32 off, qint32 len)
{
	qint32 length = src->length();
	if ((off < 0) || (len < 0) || (off + len > length))
	{
		throw IndexOutOfBoundsException();
	}

	if (len > getRemaining())
	{
		throw BufferOverflowException();
	}
	
	memcpy((char*)(mArray->constData()) + mPosition + off, src->constData(), len);
	mPosition += len;
	return this;
}

/**
 * Writes all the remaining bytes of the {@code src} byte buffer to this
 * buffer's current position, and increases both buffers' position by the
 * number of bytes copied.
 *
 * @param src
 *            the source byte buffer.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if {@code src.remaining()} is greater than this buffer's
 *                {@code remaining()}.
 * @exception IllegalArgumentException
 *                if {@code src} is this buffer.
 */
ByteBuffer * ByteBuffer::put(ByteBuffer * src)
{
	if (src == this)
	{
		throw IllegalArgumentException();
	}
	if (src->getRemaining() > getRemaining())
	{
		throw BufferOverflowException();
	}
	QByteArray* contents = new QByteArray(src->getRemaining(), 0);
	src->get(contents);
	put(contents);
	delete contents;
	return this;
}

/**
 * Write a byte to the specified index of this buffer without changing the
 * position.
 *
 * @param index
 *            the index, must not be negative and less than the limit.
 * @param b
 *            the byte to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::put(qint32 index, char b)
{
	mArray->data()[index] = b;
	return this;
}

/**
 * Writes the given char to the current position and increases the position
 * by 2.
 * <p>
 * The char is converted to bytes using the current byte order.
 *
 * @param value
 *            the char to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 2}.
 */
ByteBuffer* ByteBuffer::putChar(char value)
{
	return put(value);
}

/**
 * Writes the given char to the specified index of this buffer.
 * <p>
 * The char is converted to bytes using the current byte order. The position
 * is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 2}.
 * @param value
 *            the char to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putChar(qint32 index, char value)
{
	return put(index, value);
}

/**
 * Writes the given double to the current position and increases the position
 * by 8.
 * <p>
 * The double is converted to bytes using the current byte order.
 *
 * @param value
 *            the double to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 8}.
 */
ByteBuffer* ByteBuffer::putDouble(double value)
{
    double* _v = &value;
    qint64* _d = (qint64*)_v;
    return putLong(*_d);
}

/**
 * Writes the given double to the specified index of this buffer.
 * <p>
 * The double is converted to bytes using the current byte order. The
 * position is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 8}.
 * @param value
 *            the double to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putDouble(qint32 index, double value)
{
    double* _v = &value;
    qint64* _d = (qint64*)_v;
    return putLong(index, *_d);
}

/**
 * Writes the given float to the current position and increases the position
 * by 4.
 * <p>
 * The float is converted to bytes using the current byte order.
 *
 * @param value
 *            the float to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 4}.
 */
ByteBuffer* ByteBuffer::putFloat(float value)
{
    float* _v = &value;
    qint32* _d = (qint32*)_v;
    return putInt(*_d);
}

/**
 * Writes the given float to the specified index of this buffer.
 * <p>
 * The float is converted to bytes using the current byte order. The
 * position is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 4}.
 * @param value
 *            the float to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putFloat(qint32 index, float value)
{
    float* _v = &value;
    qint32* _d = (qint32*)_v;
    return putInt(index, *_d);
}

/**
 * Writes the given qint32 to the current position and increases the position by
 * 4.
 * <p>
 * The qint32 is converted to bytes using the current byte order.
 *
 * @param value
 *            the qint32 to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 4}.
 */
ByteBuffer* ByteBuffer::putInt(qint32 value)
{
	qint32 pos = mPosition;
	mPosition += 4;
	return putInt(pos, value);
}

/**
 * Writes the given qint32 to the specified index of this buffer.
 * <p>
 * The qint32 is converted to bytes using the current byte order. The position
 * is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 4}.
 * @param value
 *            the qint32 to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putInt(qint32 index, qint32 value)
{
	if (mOrder != ByteOrder::endianness())
	{
		value = ((((value)& 0xff000000) >> 24) | (((value)& 0x00ff0000) >> 8) | (((value)& 0x0000ff00) << 8)
			| (((value)& 0x000000ff) << 24));
	}
	(*(qint32*)(mArray->data() + index)) = value;
	return this;
}

/**
 * Writes the given long to the current position and increases the position
 * by 8.
 * <p>
 * The long is converted to bytes using the current byte order.
 *
 * @param value
 *            the long to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 8}.
 */
ByteBuffer* ByteBuffer::putLong(qint64 value)
{
	qint32 pos = mPosition;
	mPosition += 8;
	return putLong(pos, value);
}

/**
 * Writes the given long to the specified index of this buffer.
 * <p>
 * The long is converted to bytes using the current byte order. The position
 * is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 8}.
 * @param value
 *            the long to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putLong(qint32 index, qint64 value)
{
	if (mOrder != ByteOrder::endianness())
	{
		value =  ((((value)& 0xff00000000000000ull) >> 56)
				| (((value)& 0x00ff000000000000ull) >> 40)
				| (((value)& 0x0000ff0000000000ull) >> 24)
				| (((value)& 0x000000ff00000000ull) >> 8)
				| (((value)& 0x00000000ff000000ull) << 8)
				| (((value)& 0x0000000000ff0000ull) << 24)
				| (((value)& 0x000000000000ff00ull) << 40)
				| (((value)& 0x00000000000000ffull) << 56));
	}
	(*(qint64*)(mArray->data() + index)) = value;
	return this;
}

/**
 * Writes the given short to the current position and increases the position
 * by 2.
 * <p>
 * The short is converted to bytes using the current byte order.
 *
 * @param value
 *            the short to write.
 * @return this buffer.
 * @exception BufferOverflowException
 *                if position is greater than {@code limit - 2}.
 */
ByteBuffer* ByteBuffer::putShort(short value)
{
	qint32 pos = mPosition;
	mPosition += 2;
	return putShort(pos, value);
}

/**
 * Writes the given short to the specified index of this buffer.
 * <p>
 * The short is converted to bytes using the current byte order. The
 * position is not changed.
 *
 * @param index
 *            the index, must not be negative and equal or less than
 *            {@code limit - 2}.
 * @param value
 *            the short to write.
 * @return this buffer.
 * @exception IndexOutOfBoundsException
 *                if {@code index} is invalid.
 */
ByteBuffer* ByteBuffer::putShort(qint32 index, short value)
{
	if (mOrder != ByteOrder::endianness())
	{
		value = ((((value) >> 8) & 0xff) | (((value)& 0xff) << 8));
	}
	(*(qint64*)(mArray->data() + index)) = value;
	return this;
}

/**
 * Returns the number of elements between the current position and the
 * limit. </p>
 *
 * @return  The number of elements remaining in this buffer
 */
qint32 ByteBuffer::getRemaining()
{
	return mLimit - mPosition;
}

/**
 *
 */
void ByteBuffer::setOwner(bool owner)
{
	mOwner = owner;
}


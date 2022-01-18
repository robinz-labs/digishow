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
 
#include <osc/composer/OscContentComposer.h>
#include <tools/ByteBuffer.h>
#include <math.h>

qint32 OscContentComposer::mHeaderBufferSize = 128;
QByteArray OscContentComposer::mBlank(4, 0);

OscContentComposer::OscContentComposer()
{
	mHeaderBuffer = new QByteArray(mHeaderBufferSize, 0);
	mHeaderByteBuffer = ByteBuffer::wrap(mHeaderBuffer, true);
}

OscContentComposer::~OscContentComposer()
{
	// No Need to delete since buffer is owned by ByteBuffer
	// delete mHeaderBuffer;

	delete mHeaderByteBuffer;
}

qint32 OscContentComposer::computeSize()
{
	// Be sure to align type tags ending
	if (mHeaderByteBuffer->get(mHeaderByteBuffer->getPosition() - 1) != 0)
	{
		mHeaderByteBuffer = checkBufferSize(mHeaderByteBuffer, 1);
		mHeaderByteBuffer->put((char)0);
	}
	fillAlignment(mHeaderByteBuffer);
	return mHeaderByteBuffer->getPosition();
}

ByteBuffer* OscContentComposer::checkBufferSize(ByteBuffer* aBuffer, size_t aSize)
{
	if (aBuffer->getPosition() + (qint32)aSize > aBuffer->getCapacity())
	{
		qint32 newDataBufferSize =  aBuffer->getCapacity() + (qint32)pow(2, ceil(log((aSize)) / log(2)));
		ByteBuffer* newDataByteBuffer = ByteBuffer::allocate(newDataBufferSize);
		if (aBuffer->getPosition() > 0)
			newDataByteBuffer->put(aBuffer->getByteArray(), 0, aBuffer->getPosition());
		delete aBuffer;

		return newDataByteBuffer;
	}
	else
		return aBuffer;
}

void OscContentComposer::fillByteBuffer(ByteBuffer* dst)
{
	try
	{
		dst->put(mHeaderByteBuffer->getByteArray(), 0, mHeaderByteBuffer->getPosition());
	}
	catch (const QException& e)
	{
		throw e;
	}
}

void OscContentComposer::fillAlignment(ByteBuffer*& dst)
{    
    qint32 size = 4 - (dst->getPosition() & 0x3);
	if (size != 4)
	{
		try
		{
			dst = checkBufferSize(dst, size);
			dst->put(&mBlank, 0, size);
		}
		catch (const QException& e)
		{
			throw e;
		}
	}
}

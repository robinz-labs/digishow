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
 
#include <osc/composer/OscBundleComposer.h>
#include <tools/ByteBuffer.h>
#include <tools/NtpTimestamp.h>

/**
 * Build a new OSC Bundle.
 *
 * @param timetag
 *            The long value that applies as a time tag to this bundle.
 */
OscBundleComposer::OscBundleComposer(QDateTime timetag) :
OscContentComposer()
{
	mData = NULL;
	QString bundle("#bundle");
	try
	{
		QByteArray bundleByteArray(bundle.toLocal8Bit());
		mHeaderByteBuffer->put(&bundleByteArray);
		mHeaderByteBuffer->put( '\0' );
	}
	catch (const QException& e)
	{
		throw e;
	}
	//delete bundleArray;
	NtpTimestamp ntp = NtpTimestamp::fromDateTime(timetag);
	mHeaderByteBuffer->putInt( (qint32)ntp.seconds() );
	mHeaderByteBuffer->putInt( (qint32)ntp.fraction() );
}

OscBundleComposer::~OscBundleComposer() {
	for (qint32 i = 0; i < mContentList.size(); i++)
		delete mContentList[i];
}

/**
 * Appends a new message to the current bundle.
 *
 * @param address
 * @return the new message to compose.
 */
OscMessageComposer* OscBundleComposer::pushMessage(QString& address)
{
	OscMessageComposer* message = new OscMessageComposer(address);
	mContentList.push_back(message);
	return message;
}

/**
 * Appends a previously built message to the current bundle being composed.
 *
 * @param message
 *            The message to add to this bundle.
 */
void OscBundleComposer::pushMessage(OscMessageComposer* message)
{
	mContentList.push_back(message);
}

/**
 * Appends a new message to the current bundle.
 *
 * @param timeTag
 *            The time tag to apply to the new bundle.
 * @return the new bundle to compose.
 */
OscBundleComposer* OscBundleComposer::pushBundle(QDateTime timeTag)
{
	OscBundleComposer* bundle = new OscBundleComposer(timeTag);
	mContentList.push_back(bundle);
	return bundle;
}

/**
 * Appends a previously built bundle to the current bundle being composed.
 *
 * @param bundle
 *            The bundle to add to this bundle.
 */
void OscBundleComposer::pushBundle(OscBundleComposer* bundle)
{
	mContentList.push_back(bundle);
}

/*
 * @see com.cocon.osc.OscComposer#computeSize()
 */
qint32 OscBundleComposer::computeSize()
{
	qint32 size = mHeaderByteBuffer->getPosition(); // Do not call ::computeSize() because we don't want to align/fill with 0

	// Compute whole bundle size
	for (qint32 i = 0; i < mContentList.size(); i++)
	{
		size += 4; // reserve space to write this content size
		size += mContentList[i]->computeSize(); // get this content size
	}
	return size;
}

/**
 * Gets the complete OSC bundle for the current bundle being composed.
 *
 * @return the current OscMessage data.
 */
QByteArray* OscBundleComposer::getBytes(/*OscVersion::Version aVersion*/)
{
	qint32 size = computeSize();

	// Allocate the bundle buffer
	mData = new QByteArray(size, 0);

	// First, copy this bundle header
	try
	{
		ByteBuffer* writeBufferHelper = ByteBuffer::wrap(mData);
		writeBufferHelper->putInt(0, size);
		fillByteBuffer(writeBufferHelper);
		return new QByteArray(*mData);
	}
	catch (const QException& e)
	{
		throw e;
	}
}

/*
 * @see com.cocon.osc.OscComposer#fillByteBuffer(java.nio.ByteBuffer)
 */
void OscBundleComposer::fillByteBuffer(ByteBuffer* byteBuffer)
{
	try
	{
		OscContentComposer::fillByteBuffer(byteBuffer);

		// Then, loop over all the contents to copy data in the buffer
		for (qint32 i = 0; i < mContentList.size(); i++)
		{
			// Write content size
			OscContentComposer* c = mContentList[i];
			byteBuffer->putInt(c->computeSize());
			c->fillByteBuffer(byteBuffer);
		}
	}
	catch (const QException& e)
	{
		throw e;
	}
}


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
 
#ifndef OSC_BUNDLE_COMPOSER_H_
#define OSC_BUNDLE_COMPOSER_H_

#include <osc/composer/OscContentComposer.h>
#include <osc/composer/OscMessageComposer.h>

class OSC_API OscBundleComposer : public OscContentComposer
{
private:
	QByteArray* mData;
public:
	/**
	 * Build a new OSC Bundle.
	 *
	 * @param timetag
	 *            The long value that applies as a time tag to this bundle.
	 */
	OscBundleComposer(QDateTime timetag);

	/** Destructor. */
	virtual ~OscBundleComposer();

	/**
	 * Appends a new message to the current bundle.
	 *
	 * @param address
	 * @return the new message to compose.
	 */
	OscMessageComposer* pushMessage(QString& address);

	/**
	 * Appends a previously built message to the current bundle being composed.
	 *
	 * @param message
	 *            The message to add to this bundle.
	 */
	void pushMessage(OscMessageComposer* message);

	/**
	 * Appends a new message to the current bundle.
	 *
	 * @param timeTag
	 *            The time tag to apply to the new bundle.
	 * @return the new bundle to compose.
	 */
	OscBundleComposer* pushBundle(QDateTime timeTag = NtpTimestamp::IMMEDIATE);

	/**
	 * Appends a previously built bundle to the current bundle being composed.
	 *
	 * @param bundle
	 *            The bundle to add to this bundle.
	 */
	void pushBundle(OscBundleComposer* bundle);

	/**
	 * Gets the complete OSC bundle for the current bundle being composed.
	 *
	 * @return the current OscMessage data.
	 */
	virtual QByteArray* getBytes(/*OscVersion::Version aVersion*/);

protected:
	/*
	 * @see com.cocon.osc.OscComposer#computeSize()
	 */
	virtual qint32 computeSize();

	/*
	 * @see com.cocon.osc.OscComposer#fillByteBuffer()
	 */
	virtual void fillByteBuffer(ByteBuffer* byteBuffer);

protected:
	/** List of all messages or bundles to compose to build the whole Bundle. */
	QVector<OscContentComposer*> mContentList;
};

#endif // OSC_BUNDLE_COMPOSER_H_

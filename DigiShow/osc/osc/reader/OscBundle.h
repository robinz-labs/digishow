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
 
#ifndef OSC_BUNDLE_H_
#define OSC_BUNDLE_H_

#include <osc/reader/OscContent.h>
#include <QDateTime>

class OSC_API OscBundle : public OscContent {
public:

	/** TODO */
	size_t getNumEntries();

	/**
	 * Conversion type for current OscContent object as an OscBundle object.
	 *
	 * @return this as an OscBundle object. If this cannot be convert to an
	 *         OscBundle object, returns null.
	 */
	OscBundle* getBundle(qint32 index);

	/**
	 * Conversion type for current OscContent object as an OscMessage
	 * object.
	 *
	 * @return this as an OscMessage object. If this cannot be convert to an
	 *         OscMessage object, returns null.
	 */
	OscMessage* getMessage(qint32 index);

	/**
	 * Returns the current bundle time tag as a 64-bit integer value
	 *
	 * @return the time tag as a long value.
	 */
	QDateTime getTimeTag();

private:
	/**
	 * Build a new OscBundle object.
	 *
	 * @param bufferSize
	 *            The size of the buffer containing the bundle to load.
	 * @throws OscMalformedBundleException
	 *             The bundle cannot be read properly.
	 * @throws OscMalformedMessageException
	 *             A contained message cannot be read properly.
	 */
	OscBundle(ByteBuffer* packet, qint32 bufferSize);

	/** Destructor */
	virtual ~OscBundle();

	/**
	 * List of all messages or bundles directly accessible from this bundle.
	 */
	QVector<OscContent*> mContentList;

	/** Required for access to constructor. */
	friend class OscReader;
};

#endif // OSC_BUNDLE_H_

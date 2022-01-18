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
 
#ifndef OSC_MESSAGE_H_
#define OSC_MESSAGE_H_

#include <osc/reader/OscContent.h>
#include <QDateTime>

class ByteArray;
class OscValue;

/**
 * Class managing packet data as an OSC message object.
 * <p>
 * An OSC message object is composed of:
 * <ul>
 * <li>An address pattern string	(aka OscAddress)
 * <li>A type tags string			(aka OscTags)
 * <li>A list of values				(aka OscValue)
 * <li>...
 * </ul>
 * </p>
 */
class OSC_API OscMessage : public OscContent
{
public:

	/** */
	size_t getNumValues() const;

	/** */
	OscValue* getValue(size_t index) const;

	/** */
	QString getAddress() const;

	/** */
	QString getTags() const;

	/** */
	virtual QDateTime getTimeTag() const;

	/** Destructor */
	virtual ~OscMessage();

private:
	/**
	 * Build a new OscMessage object. While building the message structure,
	 * the packet buffer is parsed, and new OscValue objects are created and
	 * registered in a list, in order to be accessible later.
	 *
	 * @param messageSize
	 *            The size of the buffer containing the bundle to load.
	 * @throws OscMalformedMessageException
	 *             A contained message cannot be read properly.
	 */
	OscMessage(ByteBuffer* packet, qint32 messageSize);

	/** List of values embedded in the current OscMessage. */
	QVector<OscValue*> mValues;

	/** Required for access to constructor. */
	friend class OscBundle;
	friend class OscReader;
};

#endif // OSC_MESSAGE_H_

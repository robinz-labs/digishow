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
 
#include <osc/reader/OscReader.h>
#include <osc/reader/OscBundle.h>
#include <osc/reader/OscMessage.h>
#include <osc/exceptions/MalformedBundleException.h>
#include <osc/exceptions/GetBundleException.h>
#include <osc/exceptions/GetMessageException.h>
#include <osc/exceptions/ReadMessageException.h>
#include <tools/ByteBuffer.h>

/** */
OscBundle* OscReader::getBundle()
{
	OscBundle* bundle = dynamic_cast<OscBundle*>(mContent);
	if (bundle)
		return bundle;
	throw GetBundleException();
}

/**
 * Conversion type for current OscContent object as an OscMessage
 * object.
 *
 * @return this as an OscMessage object. If this cannot be convert to an
 *         OscMessage object, returns null.
 */
OscMessage* OscReader::getMessage()
{
	OscMessage* message = dynamic_cast<OscMessage*>(mContent);
	if (message)
		return message;
	throw GetMessageException();
}

/**
 * Build a new OscContent object based on the passed byte buffer.
 *
 * @param src
 *            the byte buffer to parse containing OSC messages
 * @param length
 *            the length of the message buffer
 * @throws OscMalformedBundleException
 *             The bundle cannot be read properly.
 * @throws OscMalformedMessageException
 *             A contained message cannot be read properly.
 */
OscReader::OscReader(QByteArray* src)
{
	init(src, 0, src->size());
}

OscReader::OscReader(QByteArray* src, qint32 offset, qint32 size)
{
	init(src, offset, size);
}

OscReader::~OscReader()
{
	// DO NOT DESTROY OSCMESSAGE OR OSCBUNDLE
	// delete mContent
	// delete mPacket;
}

void
OscReader::init(QByteArray* src, qint32 offset, qint32 size)
{
	mContent = NULL;
	mPacketSize = size;
	mPacket = ByteBuffer::wrap(src);
	try
	{
		if (mPacket->get(mPacket->getPosition()) == '#')
			mContent = new OscBundle(mPacket, mPacketSize);
		else
		{
			mPacket->setPosition(offset);
			mContent = new OscMessage(mPacket, mPacketSize);
		}
	}
	catch (...)
	{
		throw ReadMessageException();
	}
}

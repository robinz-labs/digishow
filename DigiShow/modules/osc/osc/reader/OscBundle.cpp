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
 
#include <osc/reader/OscBundle.h>
#include <tools/ByteBuffer.h>
#include <osc/reader/OscMessage.h>
#include <osc/exceptions/MalformedBundleException.h>
#include <osc/exceptions/GetBundleException.h>
#include <osc/exceptions/GetMessageException.h>
#include <tools/NtpTimestamp.h>

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
OscBundle::OscBundle(ByteBuffer* packet, qint32 bufferSize)
: OscContent(packet)
{
	if (mPacket->get() != '#' || mPacket->get() != 'b' || mPacket->get() != 'u' || mPacket->get() != 'n'
	|| mPacket->get() != 'd' || mPacket->get() != 'l' || mPacket->get() != 'e' || mPacket->get() != '\0')
		throw OscMalformedBundleException();

	// Move Packet cursor after timeTag
	mPacket->getLong();

	while (mPacket->getPosition() < mStartIdx + bufferSize)
	{
		qint32 contentSize = mPacket->getInt();
		if (mPacket->get(mPacket->getPosition()) == '#')
			mContentList.push_back(new OscBundle(packet, contentSize));
		else
			mContentList.push_back(new OscMessage(packet, contentSize));
	}
}

OscBundle::~OscBundle()
{
	delete mPacket;

	for (qint32 i = 0; i < mContentList.size(); i++)
	{
		delete (mContentList[i]);
	}
	mContentList.clear();
}

/**
 * Returns the current bundle time tag as a 64-bit integer value
 *
 * @return the time tag as a long value.
 */
QDateTime OscBundle::getTimeTag()
{
	NtpTimestamp ntp(mPacket->getInt(mStartIdx + 8), mPacket->getInt(mStartIdx + 12));
	return NtpTimestamp::toDateTime(ntp);
}

/** */
size_t OscBundle::getNumEntries()
{
	return mContentList.size();
}

/** */
OscBundle* OscBundle::getBundle(qint32 index)
{
	if (dynamic_cast<OscBundle*>(mContentList[index]) != 0)
		return dynamic_cast<OscBundle*>(mContentList[index]);
	throw GetBundleException();
}

/**
 * Conversion type for current OscContent object as an OscMessage
 * object.
 *
 * @return this as an OscMessage object. If this cannot be convert to an
 *         OscMessage object, returns null.
 */
OscMessage* OscBundle::getMessage(qint32 index)
{
	if (dynamic_cast<OscMessage*>(mContentList[index]) != 0)
		return dynamic_cast<OscMessage*>(mContentList[index]);
	throw GetMessageException();
}

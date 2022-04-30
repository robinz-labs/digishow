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
 
#include <osc/reader/OscMessage.h>
#include <osc/reader/OscReader.h>
#include <tools/ByteBuffer.h>
#include <osc/exceptions/MalformedArrayException.h>
#include <osc/exceptions/OutOfBoundsReadException.h>
#include <osc/exceptions/UnknownTagException.h>

#include <osc/reader/types/OscAddress.h>
#include <osc/reader/types/OscArrayBegin.h>
#include <osc/reader/types/OscArrayEnd.h>
#include <osc/reader/types/OscBlob.h>
#include <osc/reader/types/OscChar.h>
#include <osc/reader/types/OscDouble.h>
#include <osc/reader/types/OscFalse.h>
#include <osc/reader/types/OscFloat.h>
#include <osc/reader/types/OscInfinitum.h>
#include <osc/reader/types/OscInteger.h>
#include <osc/reader/types/OscLong.h>
#include <osc/reader/types/OscMidi.h>
#include <osc/reader/types/OscNil.h>
#include <osc/reader/types/OscRGBA.h>
#include <osc/reader/types/OscString.h>
#include <osc/reader/types/OscSymbol.h>
#include <osc/reader/types/OscTags.h>
#include <osc/reader/types/OscTimetag.h>
#include <osc/reader/types/OscTrue.h>

/** */
size_t OscMessage::getNumValues() const
{
	return mValues.size() - 2;
}

/** */
OscValue* OscMessage::getValue(size_t index) const
{
	return mValues[index + 2];
}

QString OscMessage::getAddress() const
{
	return ((OscAddress*)getValue(-2))->toString();
}

QString OscMessage::getTags() const
{
	return ((OscTags*)getValue(-1))->toString();
}

QDateTime OscMessage::getTimeTag() const
{
	int pos = getTags().indexOf( 't' );
	if (pos != -1)
	{
		OscValue* value = getValue( pos - 1 );
		return value->toTimetag();
	}
	return NtpTimestamp::IMMEDIATE;
}

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
OscMessage::OscMessage(ByteBuffer* packet, qint32 messageSize) :
OscContent(packet)
{
	qint32 arrayLevel = 0;
	try
	{
		mValues.push_back(new OscAddress(packet, mStartIdx));

		// Current packet position is set to data section.
		qint32 dataIdx = mPacket->getPosition();
		qint32 tagsIdx = dataIdx;
		do
		{
			if (dataIdx > mStartIdx + messageSize)
				throw new BufferUnderflowException();

			char tag = mPacket->get(tagsIdx);
			dataIdx = mPacket->getPosition();
			OscValue* newValue = 0;
			switch (tag)
			{
				case 0: // null tag... exit loop...
					break;
				case ',':
					newValue = new OscTags(packet, dataIdx);
					break;
				case 'T':
					newValue = new OscTrue(packet, dataIdx);
					break;
				case 'F':
					newValue = new OscFalse(packet, dataIdx);
					break;
				case 'N':
					newValue = new OscNil(packet, dataIdx);
					break;
				case 'I':
					newValue = new OscInfinitum(packet, dataIdx);
					break;
				case '[':
					newValue = new OscArrayBegin(packet, dataIdx);
					++arrayLevel;
					break;
				case ']':
					newValue = new OscArrayEnd(packet, dataIdx);
					--arrayLevel;
					break;
				case 'i':
					newValue = new OscInteger(packet, dataIdx);
					break;
				case 'f':
					newValue = new OscFloat(packet, dataIdx);
					break;
				case 'c':
					newValue = new OscChar(packet, dataIdx);
					break;
				case 'r':
					newValue = new OscRGBA(packet, dataIdx);
					break;
				case 'm':
					newValue = new OscMidi(packet, dataIdx);
					break;
				case 'h':
					newValue = new OscLong(packet, dataIdx);
					break;
				case 't':
					newValue = new OscTimetag(packet, dataIdx);
					break;
				case 'd':
					newValue = new OscDouble(packet, dataIdx);
					break;
				case 's':
					newValue = new OscString(packet, dataIdx);
					break;
				case 'S':
					newValue = new OscSymbol(packet, dataIdx);
					break;
				case 'b':
					newValue = new OscBlob(packet, dataIdx);
					break;
				default:
					throw UnknownTagException();
			}            

			if (newValue != 0)
				mValues.push_back(newValue); // push this Value to the Value list

		} while (mPacket->get(tagsIdx++) != 0);
	}
	catch (BufferUnderflowException&)
	{
		throw OutOfBoundsReadException();
	}
	catch (IndexOutOfBoundsException&)
	{
		throw OutOfBoundsReadException();
	}
	if (arrayLevel != 0)
		throw MalformedArrayException();
}

OscMessage::~OscMessage()
{
	delete mPacket;

	for (qint32 i = 0; i < mValues.size(); i++)
	{
		delete (mValues[i]);
	}
	mValues.clear();
}

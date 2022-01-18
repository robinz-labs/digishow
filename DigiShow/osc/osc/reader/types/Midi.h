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
 
#ifndef MIDI_H_
#define MIDI_H_

#include <osc/OscAPI.h>

class OSC_API Midi
{
private:
	char mPort;
	char mStatus;
	char mData1;
	char mData2;
public:
	Midi()
	{
		mPort = 'm';
		mStatus = 'i';
		mData1 = 'd';
		mData2 = 'i';
	}

	Midi(const QByteArray& aByteArray)
	{
		mPort = aByteArray[0];
		mStatus = aByteArray[1];
		mData1 = aByteArray[2];
		mData2 = aByteArray[3];
	}

	Midi(char aPort, char aStatus, char aData1, char aData2)
	{
		mPort = aPort;
		mStatus = aStatus;
		mData1 = aData1;
		mData2 = aData2;
	}

	char getPort() const
	{
		return mPort;
	}

	char getStatus() const
	{
		return mStatus;
	}

	char getData1() const
	{
		return mData1;
	}

	char getData2() const
	{
		return mData2;
	}

	bool operator== (const Midi& m) const
	{
		return mData1 == m.mData1
			&& mData2 == m.mData2
			&& mPort == m.mPort
			&& mStatus == m.mStatus;
	}
};


#endif // MIDI_H_

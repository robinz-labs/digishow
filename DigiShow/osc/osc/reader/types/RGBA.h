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
 
#ifndef RGBA_H_
#define RGBA_H_

#include <osc/OscAPI.h>

class OSC_API RGBA
{
private:
	qint32 mPacked;
public:
	RGBA()
	{
		mPacked = 0;
	}

	RGBA(const QByteArray& bytes)
	{
		mPacked = *(int*)bytes.constData();
	}

	RGBA(qint32 aPackedRGBA)
	{
		mPacked = aPackedRGBA;
	}

	RGBA(char r, char g, char b, char a)
	{
		mPacked = 0;
		mPacked &= r;
		mPacked <<= 8;
		mPacked &= g;
		mPacked <<= 8;
		mPacked &= b;
		mPacked <<= 8;
		mPacked &= a;
	}

	qint32 getPackedRGBA()
	{
		return mPacked;
	}

	void getUnpackedRGBA(qint32 &r, qint32 &g, qint32 &b, qint32 &a)
	{
        a = mPacked & 0xFF;
		mPacked >>= 8;
        b = mPacked & 0xFF;
		mPacked >>= 8;
        g = mPacked & 0xFF;
		mPacked >>= 8;
        r = mPacked & 0xFF;
	}

	bool operator== (const RGBA& rgba) const
	{
		return mPacked == rgba.mPacked;
	}
};

#endif // RGBA_H_

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

#ifndef _NTP_TIMESTAMP_H_
#define _NTP_TIMESTAMP_H_

#include <QDateTime>
#include <QtEndian>
#include <osc/OscAPI.h>

class OSC_API NtpTimestamp
{
public:

	static const QDateTime IMMEDIATE;

	NtpTimestamp()
	{
		mSeconds = qToBigEndian(0);
		mFraction = qToBigEndian(1);
	}

	/** Build a NtpTimestamp object.
	 * @param seconds	Number of seconds passed since Jan 1 1900.
	 * @param fraction	Fractional time part, in <tt>1/0xFFFFFFFF</tt>s of a second.
	 */
	NtpTimestamp( quint32 seconds, quint32 fraction )
	{
		mSeconds = qToBigEndian(seconds);
		mFraction = qToBigEndian(fraction);
	}

	inline void setSeconds(quint32 seconds);

	inline void setFraction(quint32 fraction);

	inline quint32 seconds();

	inline quint32 fraction();

	static QDateTime getImmediateTime();

	/**
	* @param dateTime                 Qt date time.
	* @returns                        Ntp time for the given qt
	*/
	static NtpTimestamp fromDateTime(const QDateTime &dateTime);

	/**
	* @param ntpTime                  NTP timestamp.
	* @returns                        Qt date time for the given NTP timestamp.
	*/
	static QDateTime toDateTime(const NtpTimestamp &ntpTime);

	bool operator== (const NtpTimestamp& m) const;

	bool operator!= (const NtpTimestamp& m) const;

	bool operator> (const NtpTimestamp& m) const;

	bool operator< (const NtpTimestamp& m) const;

	bool operator>= (const NtpTimestamp& m) const;

	bool operator<= (const NtpTimestamp& m) const;

	inline quint32& rawSeconds();

	inline quint32& rawFraction();

private:
	static const qint64 january_1_1900;

	/** Number of seconds passed since Jan 1 1900, in big-endian format. */
	quint32 mSeconds;

	/** Fractional time part, in <tt>1/0xFFFFFFFF</tt>s of a second. */
	quint32 mFraction;
};

//-------------------------------------------------------------------------------
void
NtpTimestamp::setSeconds(quint32 seconds)
{
	mSeconds = qToBigEndian(seconds);
}

//-------------------------------------------------------------------------------
void
NtpTimestamp::setFraction(quint32 fraction)
{
	mFraction = qToBigEndian(fraction);
}

//-------------------------------------------------------------------------------
quint32
NtpTimestamp::seconds()
{
	return qFromBigEndian(mSeconds);
}

//-------------------------------------------------------------------------------
quint32
NtpTimestamp::fraction()
{
	return qFromBigEndian(mFraction);
}

//-------------------------------------------------------------------------------
quint32&
NtpTimestamp::rawSeconds()
{
	return mSeconds;
}

//-------------------------------------------------------------------------------
quint32&
NtpTimestamp::rawFraction()
{
	return mFraction;
}
#endif

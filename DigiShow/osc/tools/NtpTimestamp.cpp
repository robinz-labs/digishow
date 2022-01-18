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

#include <tools/NtpTimestamp.h>
#include <QtEndian>

const qint64 NtpTimestamp::january_1_1900 = -2208988800000ll;

const QDateTime NtpTimestamp::IMMEDIATE = NtpTimestamp::getImmediateTime();

QDateTime
NtpTimestamp::getImmediateTime()
{
	/* Convert to local-endian. */
	qint64 seconds = 0;
	qint64 fraction = qToBigEndian(1);

	/* Convert NTP timestamp to number of milliseconds passed since Jan 1 1900. */
	qint64 ntpMSecs = seconds * 1000 + (fraction * 1000) / 0x100000000;

	/* Construct Qt date time. */
	return QDateTime::fromMSecsSinceEpoch(ntpMSecs + NtpTimestamp::january_1_1900);
}

NtpTimestamp
NtpTimestamp::fromDateTime(const QDateTime &dateTime)
{
	/* Convert given time to number of milliseconds passed since Jan 1 1900. */
	quint64 ntpMSecs = dateTime.toMSecsSinceEpoch() - NtpTimestamp::january_1_1900;

	/* Note that NTP epoch information is lost here. Assume 1900-2036 NTP epoch. */
	quint32 seconds = ntpMSecs / 1000;
	quint32 fraction = (0x100000000 * (ntpMSecs % 1000)) / 1000;

	/* Convert to big-endian. */
	NtpTimestamp result;
	result.mSeconds = qToBigEndian(seconds);
	result.mFraction = qToBigEndian(fraction);
	return result;
}

QDateTime
NtpTimestamp::toDateTime(const NtpTimestamp &ntpTime)
{
	/* Convert to local-endian. */
	quint32 seconds = qFromBigEndian(ntpTime.mSeconds);
	quint32 fraction = qFromBigEndian(ntpTime.mFraction);

	/* Convert NTP timestamp to number of milliseconds passed since Jan 1 1900. */ 
	quint64 ntpMSecs = (quint64)seconds * 1000 + (quint64)fraction * 1000 / 0x100000000;

	/* Construct Qt date time. */
	return QDateTime::fromMSecsSinceEpoch(ntpMSecs + NtpTimestamp::january_1_1900);
}

bool
NtpTimestamp::operator== (const NtpTimestamp& m) const
{
	return (mSeconds == m.mSeconds
	&& mFraction == m.mFraction);
}

bool
NtpTimestamp::operator!= (const NtpTimestamp& m) const
{
	return (mSeconds != m.mSeconds
	|| mFraction != m.mFraction);
}

bool
NtpTimestamp::operator> (const NtpTimestamp& m) const
{
	return (NtpTimestamp::toDateTime(*this) > NtpTimestamp::toDateTime(m));
}

bool
NtpTimestamp::operator< (const NtpTimestamp& m) const
{
	return (NtpTimestamp::toDateTime(*this) < NtpTimestamp::toDateTime(m));
}

bool
NtpTimestamp::operator>= (const NtpTimestamp& m) const
{
	return (NtpTimestamp::toDateTime(*this) >= NtpTimestamp::toDateTime(m));
}

bool
NtpTimestamp::operator<= (const NtpTimestamp& m) const
{
	return (NtpTimestamp::toDateTime(*this) <= NtpTimestamp::toDateTime(m));
}

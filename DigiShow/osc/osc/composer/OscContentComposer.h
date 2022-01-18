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
 
#ifndef OSC_COMPOSER_H_
#define OSC_COMPOSER_H_

#include <osc/OscVersion.h>
#include <QtCore/QByteArray>
#include <QtCore/QVector>

class ByteBuffer;

class OSC_API OscContentComposer
{
protected:
	virtual ~OscContentComposer();

	/**
	 * Returns the size of the current OscContentComposer.
	 *
	 * @return the current message size
	 */
	virtual qint32 computeSize();

	/**
	 * Copy current message buffers to passed ByteBuffer
	 *
	 * @param dst
	 *            The ByteBuffer where to copy current data.
	 */
	virtual void fillByteBuffer(ByteBuffer* dst);

	/**
	 * Complete passed ByteBuffer to the next 4-bytes alignment.
	 *
	 * @param dst
	 *            The ByteBuffer to complete
	 */
	static void fillAlignment(ByteBuffer*& dst);

	ByteBuffer* mHeaderByteBuffer;

	virtual QByteArray* getBytes(/*OscVersion::Version aVersion */) = 0;

	/**
	 * Build a new Composer. By default, bytes are written in BIG_ENDIAN style.
	 */
	OscContentComposer();

	/**
	 *
	 */
	static ByteBuffer* checkBufferSize(ByteBuffer* aBuffer, size_t aSize);

private:
	QByteArray* mHeaderBuffer;
	static qint32 mHeaderBufferSize;
	static QByteArray mBlank;

	friend class OscBundleComposer;
};

#endif // OSC_COMPOSER_H_

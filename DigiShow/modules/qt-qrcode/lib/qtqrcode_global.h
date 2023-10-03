/****************************************************************************
 *
 * Copyright (c) 2015 Daniel San, All rights reserved.
 * 
 *    Contact: daniel.samrocha@gmail.com
 *       File: qtqrcode_global.h
 *     Author: daniel
 * Created on: 03/02/2015
 *    Version: 
 *
 * This file is part of the Qt QRCode library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 *
 ***************************************************************************/

#ifndef QTQRCODE_GLOBAL_H
#define QTQRCODE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTQRCODE_LIBRARY)
#  define QTQRCODESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTQRCODESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTQRCODE_GLOBAL_H

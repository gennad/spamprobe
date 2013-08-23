///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: AbstractMultiLineString.cc 272 2007-01-06 19:37:27Z brian $
//
// Copyright (C) 2007 Burton Computer Corporation
// ALL RIGHTS RESERVED
//
// This program is open source software; you can redistribute it
// and/or modify it under the terms of the Q Public License (QPL)
// version 1.0. Use of this software in whole or in part, including
// linking it (modified or unmodified) into other programs is
// subject to the terms of the QPL.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Q Public License for more details.
//
// You should have received a copy of the Q Public License
// along with this program; see the file LICENSE.txt.  If not, visit
// the Burton Computer Corporation or CoolDevTools web site
// QPL pages at:
//
//    http://www.burton-computer.com/qpl.html
//    http://www.cooldevtools.com/qpl.html
//

#include "SimpleMultiLineStringCharReader.h"
#include "AbstractMultiLineString.h"

AbstractMultiLineString::~AbstractMultiLineString()
{
}

Ref<AbstractCharReader> AbstractMultiLineString::createCharReader() const
{
    return Ref<AbstractCharReader>(new SimpleMultiLineStringCharReader(this, EMPTY_STRING));
}

Ref<AbstractCharReader> AbstractMultiLineString::createCharReader(const string &terminator) const
{
    return Ref<AbstractCharReader>(new SimpleMultiLineStringCharReader(this, terminator));
}

const string &AbstractMultiLineString::join(string &buffer) const
{
    return join(buffer, EMPTY_STRING);
}

const string &AbstractMultiLineString::join(string &buffer, const string &terminator) const
{
    for (int i = 0; i < lineCount(); ++i) {
        buffer += line(i);
        buffer += terminator;
    }
    return buffer;
}


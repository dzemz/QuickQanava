/*
    This file is part of Quick Qanava library.

    Copyright (C) 2008-2015 Benoit AUTHEMAN

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	qanProgressNotifier.cpp
// \author	benoit@destrat.io
// \date	2016 02 15
//-----------------------------------------------------------------------------

// QuickQanava headers
#include "./qanProgressNotifier.h"

namespace qan { // ::qan

ProgressNotifier::ProgressNotifier( QObject* parent ) :
    QObject( parent ),
    gtpo::ProgressNotifier( nullptr )
{
}

} // ::qan


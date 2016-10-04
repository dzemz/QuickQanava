/*
    This file is part of QuickGeoGL library.

    Copyright (C) 2016 Benoit AUTHEMAN

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
// This file is a part of the QuickGeoGL software. Copyright (C) 2016 Benoit Autheman.
//
// \file	qglArrow.cpp
// \author	benoit@destrat.io
// \date	2016 09 29
//-----------------------------------------------------------------------------

// Std headers
#include <cmath>    // std::acos()

// Qt headers
#include <QQuickItem>
#include <QSGGeometryNode>
#include <QLineF>

// QuickGeoGL headers
#include "./qglLine.h"
#include "./qglArrow.h"

namespace qgl { // ::qgl

/* Arrow Object Management *///------------------------------------------------
Arrow::Arrow( QQuickItem* parent ) :
    qgl::Line{ parent }
{
    _line = std::make_unique<qgl::Line>(this);
    updateCapStyle();   // Force creation of sub cap quick items with default settings.
    setFlag( ItemHasContents, true );
}

auto    Arrow::setColor( QColor color ) noexcept -> void
{
    if ( color != _color ) {    // Binding loop protection
        _color = color;
        if ( _line != nullptr )
            _line->setColor( color );
        if ( _p1Arrow != nullptr )
            _p1Arrow->setColor( color );
        if ( _p2Arrow != nullptr )
            _p2Arrow->setColor( color );
        emit colorChanged();
    }
}

auto    Arrow::setP1( QPointF p1 ) noexcept -> void
{
    _p1 = p1;
    updateGeometry();
    emit p1Changed();
}

auto    Arrow::setP2( QPointF p2 ) noexcept -> void
{
    _p2 = p2;
    updateGeometry();
    emit p2Changed();
}

auto    Arrow::setLineWidth( qreal lineWidth ) noexcept -> void
{
    _line->setLineWidth( lineWidth );
    if ( getP2CapSize() < lineWidth / 2. )
        setP2CapSize( lineWidth / 2. );
    if ( getP1CapSize() < lineWidth / 2. )
        setP1CapSize( lineWidth / 2. );
    emit lineWidthChanged();
}

auto    Arrow::updateGeometry() noexcept -> void
{
    QLineF line{ _p1, _p2 };
    qreal lineLength = line.length( );
    if ( lineLength < 0.0001 )
        return;

    switch ( getP1CapStyle() ) {        // Source Cap
    case NoCap:
        _line->setP1( _p1 );
        break;
    case ArrowCap:
        if ( _p1Arrow != nullptr ) {
            double angle = std::acos( line.dx( ) / lineLength );
            if ( line.dy( ) <= 0 )
                angle = 2.0 * Pi - angle;
            // Correct line length to avoid generating artifacts when drawing line "under" arrow polygon
            QPointF truncatedSource{ line.pointAt( _p1CapSize * 2 / lineLength ) };
            _line->setP1( truncatedSource );
            _p1Arrow->setX( _p1.x() );
            _p1Arrow->setY( _p1.y() );
            _p1Arrow->setTransformOrigin( QQuickItem::TopLeft );
            _p1Arrow->setRotation( angle * 180 / Pi );
        }
        break;
    case CircleCap:
        break;
    }

    switch ( getP2CapStyle() ) {   // Destination Cap
    case NoCap:
        _line->setP2( _p2 );
    break;
    case ArrowCap:
        if ( _p2Arrow != nullptr ) {
            double angle = std::acos( line.dx( ) / lineLength );
            if ( line.dy( ) <= 0 )
                angle = 2.0 * Pi - angle;
            // Correct line length to avoid generating artifacts when drawing line "under" arrow polygon
            QPointF truncatedP2{ line.pointAt( 1.0 - ( _p2CapSize * 2 / lineLength ) ) };
            _line->setP2( truncatedP2 );
            _p2Arrow->setX( _p2.x() );
            _p2Arrow->setY( _p2.y() );
            _p2Arrow->setTransformOrigin( QQuickItem::TopLeft );
            _p2Arrow->setRotation( angle * 180 / Pi );
        }
        break;
    case CircleCap:
        break;
    }
}

auto    Arrow::setP1CapStyle( CapStyle p1CapStyle ) noexcept -> void
{
    if ( p1CapStyle != _p1CapStyle ) {  // Binding loop protection
        _p1CapStyle = p1CapStyle;
        emit p1CapStyleChanged();
        updateCapStyle();
        updateGeometry();
    }
}

auto    Arrow::setP2CapStyle( CapStyle p2CapStyle ) noexcept -> void
{
    if ( p2CapStyle != _p2CapStyle ) {  // Binding loop protection
        _p2CapStyle = p2CapStyle;
        emit p2CapStyleChanged();
        updateCapStyle();
        updateGeometry();
    }
}

auto    Arrow::setP1CapSize( qreal capSize ) noexcept -> void
{
    capSize = qMax( getLineWidth() / 2., capSize );  // capSize can't be < than line width
    if ( !qFuzzyCompare( 1.0 + capSize, 1.0 + _p1CapSize ) ) {     // Binding loop protection
        _p1CapSize = capSize;
        emit p1CapSizeChanged();
        updateCapStyle();   // Update internal cap geometry
        updateGeometry();   // Arrow line length is modified when cap size change
    }
}

auto    Arrow::setP2CapSize( qreal capSize ) noexcept -> void
{
    capSize = qMax( getLineWidth() / 2., capSize );  // capSize can't be < than line width
    if ( !qFuzzyCompare( 1.0 + capSize, 1.0 + _p2CapSize ) ) {     // Binding loop protection
        _p2CapSize = capSize;
        emit p2CapSizeChanged();
        updateCapStyle();   // Update internal cap geometry
        updateGeometry();   // Arrow line length is modified when cap size change
    }
}

auto    Arrow::updateCapStyle() noexcept -> void
{
    switch ( getP1CapStyle() ) {
    case NoCap:
        if ( _p1Arrow != nullptr )
            _p1Arrow.reset();
        break;
    case ArrowCap:
        if ( _p1Arrow == nullptr )
            _p1Arrow = std::make_unique<qgl::ConvexPolygon>( this );
        if ( _p1Arrow != nullptr ) {
            _p1Arrow->setVisible(true);
            _p1Arrow->getBorder()->setWidth(2.0);
            //_p1Arrow->getBorder()->setColor(_color); // FIXME
            double arrowLength = _p1CapSize * 2.;
            QPolygonF p{ { { 0, 0},
                           { arrowLength, -_p1CapSize },
                           { arrowLength, _p1CapSize },
                           { 0, 0 } }
                       };
            _p1Arrow->setPolygon( p );
        }
        break;
    case CircleCap:
        if ( _p1Arrow != nullptr )
            _p1Arrow->setVisible( false );
        // Note 20160930: Circle cap are actually unsupproted
        break;
    }

    switch ( getP2CapStyle() ) {
    case NoCap:
        if ( _p2Arrow != nullptr )
            _p2Arrow.reset();
        break;
    case ArrowCap:
        if ( _p2Arrow == nullptr )
            _p2Arrow = std::make_unique<qgl::ConvexPolygon>( this );
        if ( _p2Arrow != nullptr ) {
            _p2Arrow->setVisible(true);
            _p2Arrow->getBorder()->setWidth(2.0);
            //_p2Arrow->getBorder()->setColor(_color); // FIXME
            double arrowLength = _p2CapSize * 2.;
            QPointF dst{ -arrowLength, 0. };
            QPolygonF p{ { { dst.x( ), dst.y( ) - _p2CapSize },
                           { dst.x( ) + arrowLength, dst.y( ) },
                           { dst.x( ), dst.y( ) + _p2CapSize },
                           { dst.x( ), dst.y( ) - _p2CapSize } }
                       };
            _p2Arrow->setPolygon( p );
        }
        break;
    case CircleCap:
        if ( _p2Arrow != nullptr )
            _p2Arrow->setVisible( false );
        // Note 20160930: Circle cap are actually unsupproted
        break;
    }
}
//-----------------------------------------------------------------------------

} // ::qgl
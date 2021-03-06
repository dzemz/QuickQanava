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
// \file	qanNode.h
// \author	benoit@destrat.io
// \date	2004 February 15
//-----------------------------------------------------------------------------

#ifndef qanNode_h
#define qanNode_h

// QT headers
#include <QQuickItem>
#include <QPointF>
#include <QPolygonF>
#include <QDrag>

// QuickQanava headers
#include "./qanConfig.h"
#include "./qanEdge.h"
#include "./qanStyle.h"
#include "./qanGroup.h"
#include "./qanBehaviour.h"

//! Main QuickQanava namespace
namespace qan { // ::qan

class Graph;
class Group;

/*! \brief Base class for modelling nodes with attributes and an in/out edges list in a qan::Graph graph.
 *
 * \note If your application does not need drag'n'drop support (ie group insertion via dra'n'drop or ConnectorDropNode are not used nor necessary), consider disabling
 * drag'n'drop support by setting the \c acceptsDrops and \c droppable properties to false, it could improve performances significantly.
 *
 * \nosubgrouping
*/
class Node : public gtpo::GenNode< qan::Config >
{
    /*! \name Node Object Management *///--------------------------------------
    //@{
    Q_OBJECT
public:
    //! Node constructor.
    explicit Node( QQuickItem* parent = 0 );
    /*! Remove any childs node who have no QQmlEngine::CppOwnership.
     *
     */
    virtual ~Node( );
    Node( const Node& ) = delete;
public:
    inline auto         getClassName() const noexcept -> std::string { return getDynamicClassName(); }
    virtual std::string getDynamicClassName() const noexcept { return "qan::Node"; }
public:
    //! Shortcut to gtpo::GenNode<>::getGraph().
    qan::Graph*     getGraph() noexcept;
public:
    /*!
     * \note only label is taken into account for equality comparison.
     */
    bool    operator==( const qan::Node& right ) const;
public:
    // Qt property for gtpo::Node serializable standard property.
    Q_PROPERTY( bool serializable READ getSerializable WRITE setSerializableObs NOTIFY serializableChanged FINAL )
    void            setSerializableObs( bool serializable ) { setSerializable( serializable ); emit serializableChanged( ); }
signals:
    void            serializableChanged();
public:
    // Qt property for gtpo::Node resizable standard property.
    Q_PROPERTY( bool resizable READ getResizable WRITE setResizableObs NOTIFY resizableChanged FINAL )
    void            setResizableObs( bool resizable ) { setResizable( resizable ); emit resizableChanged( ); }
signals:
    void            resizableChanged();

public:
    //! Node minimum size (it can't be resized below if resizable is true).
    Q_PROPERTY( QSizeF minimumSize READ getMinimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged FINAL )
    QSizeF          getMinimumSize() const { return _minimumSize; }
    void            setMinimumSize( QSizeF minimumSize ) { _minimumSize = minimumSize; emit minimumSizeChanged( ); }
private:
    QSizeF          _minimumSize = QSizeF{ 100., 45. };
signals:
    void            minimumSizeChanged();

public:
    //! Read-only abstract item model of this node in nodes.
    Q_PROPERTY( QAbstractItemModel* inNodes READ qmlGetInNodes CONSTANT FINAL )
    QAbstractItemModel* qmlGetInNodes( ) const { return const_cast<QAbstractItemModel*>( static_cast< const QAbstractItemModel* >( &getInNodes() ) ); }

public:
    //! Read-only abstract item model of this node out nodes.
    Q_PROPERTY( QAbstractItemModel* outNodes READ qmlGetOutNodes CONSTANT FINAL )
    QAbstractItemModel* qmlGetOutNodes() const { return const_cast< QAbstractItemModel* >( qobject_cast< const QAbstractItemModel* >( &getOutNodes() ) ); }

public:
    //! Read-only abstract item model of this node out nodes.
    Q_PROPERTY( QAbstractItemModel* outEdges READ qmlGetOutEdges CONSTANT FINAL )
    QAbstractItemModel* qmlGetOutEdges() const { return const_cast< QAbstractItemModel* >( qobject_cast< const QAbstractItemModel* >( &getOutEdges() ) ); }
    //@}
    //-------------------------------------------------------------------------

    /*! \name Selection Management *///----------------------------------------
    //@{
protected slots:
    virtual void    onWidthChanged();
    virtual void    onHeightChanged();

public:
    //! Set this property to false to disable node selection (default to true, ie node are selectable by default).
    Q_PROPERTY( bool selectable READ getSelectable WRITE setSelectable NOTIFY selectableChanged FINAL )
    void            setSelectable( bool selectable );
    inline bool     getSelectable( ) const { return _selectable; }
    inline bool     isSelectable( ) const { return _selectable; }
private:
    bool            _selectable = true;
signals:
    void            selectableChanged( );

public:
    Q_PROPERTY( bool selected READ getSelected WRITE setSelected NOTIFY selectedChanged FINAL )
    //! FIXME: Actually, selected state cannot be set programmatically.
    void            setSelected( bool selected );
    inline bool     getSelected( ) const { return _selected; }
private:
    bool            _selected = false;
signals:
    void            selectedChanged( );

public:
    /*! Item used to hilight selection (usually a Rectangle quick item).
     *
     */
    Q_PROPERTY( QQuickItem* selectionItem READ getSelectionItem WRITE setSelectionItem NOTIFY selectionItemChanged FINAL )
    inline QQuickItem*  getSelectionItem() { return _selectionItem.get(); }
    void                setSelectionItem( QQuickItem* selectionItem );
private:
    std::unique_ptr< QQuickItem >  _selectionItem{ nullptr };
signals:
    void                selectionItemChanged();

public:
    //! Update selection hilight item with new color, border weight and margin.
    void            configureSelectionItem( QColor selectionColor, qreal selectionWeight, qreal selectionMargin );

    //! Update selection hilight item with new border weight and margin.
    void            configureSelectionItem( qreal selectionWeight, qreal selectionMargin );
    //@}
    //-------------------------------------------------------------------------

    /*! \name Behaviours Management *///---------------------------------------
    //@{
public:
    virtual void    installBehaviour( std::unique_ptr<qan::NodeBehaviour> behaviour );
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node DnD Management *///-----------------------------------------
    //@{
public:
    /*! Define if the node could actually be dragged by mouse.
     *
     * Set this property to true if you want to allow this node to be moved by mouse (if false, the node position is
     * fixed and should be changed programmatically).
     * Default to true.
     */
    Q_PROPERTY( bool draggable READ getDraggable WRITE setDraggable NOTIFY draggableChanged FINAL )
    void            setDraggable( bool draggable ) { _draggable = draggable; emit draggableChanged( ); }
    inline bool     getDraggable( ) const { return _draggable; }
private:
    bool            _draggable = true;
signals:
    void            draggableChanged( );

public:
    /*! Define if the node could actually be dropped in another node or in a node group.
     *
     * Set this property to true if you want to allow this node to be dropped in a qan::Group automatically.
     * Default to true.
     * Setting this property to false may lead to a significant performance improvement if group dropping is not needed.
     */
    Q_PROPERTY( bool dropable READ getDropable WRITE setDropable NOTIFY dropableChanged FINAL )
    void             setDropable( bool dropable ) { _dropable = dropable; emit dropableChanged( ); }
    inline bool     getDropable( ) const { return _dropable; }
private:
    bool            _dropable = true;
signals:
    void            dropableChanged( );

public:
    /*! Define if the node actually accept drops from other node (default to true).
     *
     * This property allow use of DropNode component that is a node droppable on another node (that has acceptDrops=true),
     * it it actually used for EdgeCreatorDropNode component to dynamically connect edges.
     *
     * Default to true.
     *
     * Setting this property to false may lead to a significant performance improvement if DropNode support is not needed.
     */
    Q_PROPERTY( bool acceptDrops READ getAcceptDrops WRITE setAcceptDrops NOTIFY acceptDropsChanged FINAL )
    void             setAcceptDrops( bool acceptDrops ) { _acceptDrops = acceptDrops; setFlag( QQuickItem::ItemAcceptsDrops, acceptDrops ); emit acceptDropsChanged( ); }
    bool             getAcceptDrops( ) const { return _acceptDrops; }
private:
    bool            _acceptDrops = true;
signals:
    void            acceptDropsChanged( );

protected:
    //! Internally used to manage drag and drop over nodes, override with caution, and call base class implementation.
    virtual void    dragEnterEvent( QDragEnterEvent* event ) override;
    //! Internally used to manage drag and drop over nodes, override with caution, and call base class implementation.
    virtual void    dragMoveEvent( QDragMoveEvent* event ) override;
    //! Internally used to manage drag and drop over nodes, override with caution, and call base class implementation.
    virtual void    dragLeaveEvent( QDragLeaveEvent* event ) override;
    //! Internally used to accept style drops.
    virtual void    dropEvent( QDropEvent* event ) override;

    virtual void    mouseDoubleClickEvent(QMouseEvent* event ) override;
    virtual void    mouseMoveEvent(QMouseEvent* event ) override;
    virtual void    mousePressEvent(QMouseEvent* event ) override;
    virtual void    mouseReleaseEvent(QMouseEvent* event ) override;

public:
    //! \c dragInitialMousePos in window coordinate system.
    inline  auto    beginDragMove( const QPointF& dragInitialMousePos, bool dragSelection = true ) -> void;
    //! \c delta in scene coordinate system.
    inline  auto    dragMove( const QPointF& dragInitialMousePos, const QPointF& delta, bool dragSelection = true ) -> void;
    inline  auto    endDragMove( bool dragSelection = true ) -> void;

public:
    //! Used internally for multiple selection dragging, contain scene position of the node at the beggining of a drag operation.
    auto            getDragInitialPos() const -> const QPointF& { return _dragInitialPos; }

private:
    //! Initial global mouse position at the beginning of a node drag operation.
    QPointF         _dragInitialMousePos{ 0., 0. };
    //! Node position at the beginning of a node drag.
    QPointF         _dragInitialPos{ 0., 0. };
    //! Last group hovered during a node drag (cached to generate a dragLeave signal on qan::Group).
    QPointer< qan::Group >  _lastProposedGroup{ nullptr };

public:
    //! True when the node is currently beeing dragged.
    Q_PROPERTY( bool dragged READ getDragActive WRITE setDragActive NOTIFY dragActiveChanged FINAL )
    void             setDragActive( bool dragActive ) { _dragActive = dragActive; emit dragActiveChanged( ); }
    bool             getDragActive( ) const { return _dragActive; }
private:
    bool            _dragActive = false;
signals:
    void            dragActiveChanged( );
    //@}
    //-------------------------------------------------------------------------

    /*! \name Appearance Management *///---------------------------------------
    //@{
private:
    using SharedNodeStyle = QSharedPointer< qan::NodeStyle >;
    SharedNodeStyle _defaultStyle;
public:
    //! Node current style object (this property is never null, a default style is returned when no style has been manually set).
    Q_PROPERTY( qan::NodeStyle* style READ getStyle WRITE setStyle NOTIFY styleChanged FINAL )
    void            setStyle( NodeStyle* style );
    qan::NodeStyle* getStyle( ) const { return _style.data(); }
private:
    QPointer<qan::NodeStyle>    _style;
signals:
    void            styleChanged( );
private slots:
    //! Called when the style associed to this node is destroyed.
    void            styleDestroyed( QObject* style );

public:
    Q_PROPERTY( QString label READ getLabel WRITE setLabel NOTIFY labelChanged FINAL )
    void            setLabel( const QString& label ) { _label = label; emit labelChanged( ); }
    QString         getLabel( ) const { return _label; }
private:
    QString         _label = QString{ "" };
signals:
    void            labelChanged( );
    //@}
    //-------------------------------------------------------------------------

    /*! \name Intersection Shape Management *///-------------------------------
    //@{
signals:
    //! Emmited whenever the node is clicked (even at the start of a dragging operation).
    void    nodeClicked( qan::Node* node, QPointF p );
    //! Emmited whenever the node is double clicked.
    void    nodeDoubleClicked( qan::Node* node, QPointF p );
    //! Emmited whenever the node is right clicked.
    void    nodeRightClicked( qan::Node* node, QPointF p );

public:
    //! Call base implementation, used internally to maintain node bounding shape.
    virtual void    geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry ) override;

public:
    /*! Polygon used for mouse event clipping, and edge arrow clipping.
     *
     * An intersection shape is automatically generated for rectangular nodes, it can be sets by the user
     * manually with setIntersectionShape() or setIntersectionShapeFromQml() if the node graphical representation is
     * not a rectangle.
     * \sa \ref custom
     */
    Q_PROPERTY( QPolygonF boundingShape READ getBoundingShape WRITE setBoundingShape NOTIFY boundingShapeChanged FINAL )
    QPolygonF           getBoundingShape();
    void                setBoundingShape( const QPolygonF& boundingShape ) { _boundingShape = boundingShape; emit boundingShapeChanged(); }
signals:
    void                boundingShapeChanged();
    //! signal is emmited when the bounding shape become invalid and should be regenerated from QML.
    void                updateBoundingShape();
protected:
    QPolygonF           generateDefaultBoundingShape() const;
    //! Generate a default bounding shape (rounded rectangle) and set it as current bounding shape.
    Q_INVOKABLE void    setDefaultBoundingShape();
private:
    QPolygonF           _boundingShape;
protected:
    /*! Invoke this method from a concrete node component in QML for non rectangular nodes.
     * \code
     * // In a component "inheriting" from QanNode:
     *
     * // Define a property in your component
     * property var polygon: new Array( )
     *
     * // In Component.onCompleted():
     * polygon.push( Qt.point( 0, 0 ) )
     * polygon.push( Qt.point( 10, 0 ) )
     * polygon.push( Qt.point( 10, 10 ) )
     * polygon.push( Qt.point( 0, 10 ) )
     * polygon.push( Qt.point( 0, 0 ) )
     * setBoundingShapeFrom( polygon );
     * \endcode
     *
     * \sa isInsideBoundingShape()
     * \ref custom
     */
    Q_INVOKABLE virtual void    setBoundingShape( QVariantList boundingPolygon );

    /*! Test if a point in the node local CCS is inside the current intersection shape.
     *
     * Usefull to accept or reject mouse drag event in QML custom node components.
     * \code
     *  // In the MouseArea component used to drag your node component (with drag.target sets correctly):
     *  onPressed : {
     *   mouse.accepted = ( isInsideBoundingShape( Qt.point( mouse.x, mouse.y ) ) ? true : false )
     *  }
     * \endcode
     *  \ref custom
     *  \sa setBoundShapeFrom()
     */
    Q_INVOKABLE virtual bool    isInsideBoundingShape( QPointF p );
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Group Management *///---------------------------------------
    //@{
public:
    /*! Ungroup this node from its current group.
     *
     * Method can be called even if the node is not actually part of a group.
     * \sa qan::Group::ungroup()
     * \sa qan::Graph::insertNode()
     */
    Q_INVOKABLE virtual void    ungroup( );

    Q_INVOKABLE qan::Group*     qmlGetGroup( ) { return getQanGroup(); }

    //! Shortcut to gtpo::GenNode<>::getGroup().
    qan::Group*                 getQanGroup( );
    //@}
    //-------------------------------------------------------------------------
};

} // ::qan

QML_DECLARE_TYPE( qan::Node )
Q_DECLARE_METATYPE( std::shared_ptr<qan::Node> )
Q_DECLARE_METATYPE( std::weak_ptr<qan::Node> )

#endif // qanNode_h

#include "graphicsview.h"
#include "qgraphicsitem.h"
#include <QScrollBar>

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setMouseTracking(true);
}

void GraphicsView::mirrorH()
{
    for (QGraphicsItem *item : items())
    {
        //        if (item->transform().m22() == 1)
        //            item->moveBy(0, item->boundingRect().height());
        //        else
        //            item->moveBy(0, -item->boundingRect().height());

        QGraphicsPixmapItem *imageLabel = dynamic_cast<QGraphicsPixmapItem *>(item);
        if (imageLabel == nullptr)
            continue;

        imageLabel->setPixmap(imageLabel->pixmap().transformed(QTransform().scale(1, -1)));
//        item->setTransform(item->transform().scale(1, -1));
    }
}

void GraphicsView::mirrorV()
{
    for (QGraphicsItem *item : items())
    {
        //        if (item->transform().m11() == 1)
        //            item->moveBy(item->boundingRect().width(), 0);
        //        else
        //            item->moveBy(-item->boundingRect().width(), 0);

        QGraphicsPixmapItem *imageLabel = dynamic_cast<QGraphicsPixmapItem *>(item);
        if (imageLabel == nullptr)
            continue;
        imageLabel->setPixmap(imageLabel->pixmap().transformed(QTransform().scale(-1, 1)));
//        item->setTransform(item->transform().scale(-1, 1));
    }
}

void GraphicsView::zoom(float val)
{
    scale(val, val);
}

void GraphicsView::setAngle(qreal angle)
{
    for (QGraphicsItem *item : items())
    {
        qreal x = item->boundingRect().width() / 2;
        qreal y = item->boundingRect().height() / 2;

        item->setTransform(QTransform().translate(x, y).rotate(angle).translate(-x, -y));
        //        item->setRotation(angle);
    }
}

QTransform GraphicsView::getTransformItem(qsizetype index)
{
    if (items().size() <= index)
        return {};

    return items().at(index)->transform();
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        QPointF subPointF = _mousePressPoint - event->position();
        horizontalScrollBar()->setValue(_scroolH + subPointF.x());
        verticalScrollBar()->setValue(_scroolV + subPointF.y());
    }

    emit signalMouseMove(event->position());
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    _mousePressPoint = event->position();
    _scroolH = horizontalScrollBar()->value();
    _scroolV = verticalScrollBar()->value();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{

}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit signalDoubleClick();
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        zoom(1.1);
    else if (event->angleDelta().y() < 0)
        zoom(0.9);
}


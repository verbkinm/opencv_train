#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView
{
     Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);

    void mirrorH();
    void mirrorV();

    void zoom(float val);
    void setAngle(qreal angle);
    QTransform getTransformItem(qsizetype index);

private:
    QPointF _mousePressPoint;
    int _scroolH, _scroolV;

    // QWidget interface
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual void wheelEvent(QWheelEvent *event) override;

signals:
    void signalDoubleClick();
    void signalMouseMove(QPointF point);
};

#endif // GRAPHICSVIEW_H

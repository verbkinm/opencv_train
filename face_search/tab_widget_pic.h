#ifndef TAB_WIDGET_PIC_H
#define TAB_WIDGET_PIC_H

#include <QWidget>
#include <QGraphicsScene>
#include "Converter_Qt_Opencv.h"

namespace Ui {
class Tab_Widget_Pic;
}

class Tab_Widget_Pic : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Widget_Pic(QWidget *parent = nullptr);
    ~Tab_Widget_Pic();

    void setImage(const QImage &img);

    void detect(DETECT_TYPE type);

private slots:
    void on_open_clicked();

    void on_zoomIn_clicked();
    void on_zoomOut_clicked();

    void slotAdjustSize();

    void on_rotate_plus45_clicked();
    void on_rotate_minus45_clicked();

    void on_save_clicked();

    void slotViewTransformedImage();

signals:
    void signalMouseMove(QPointF point);

private:
    QImage tansformOriginalImage();
    void endablePanelButtons(bool state);
    void newWindowWithDetecetObj(const cv::Mat &mat, int countObject);


    Ui::Tab_Widget_Pic *ui;

    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_pixmapItem;

    QString _lastPath;
    QImage _img;
//    cv::Mat _mat;

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // TAB_WIDGET_PIC_H

#ifndef TAB_WIDGET_PIC_H
#define TAB_WIDGET_PIC_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class Tab_Widget_Pic;
}

class Tab_Widget_Pic : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Widget_Pic(QWidget *parent = nullptr);
    ~Tab_Widget_Pic();

    void setPixmap(const QPixmap &pix);

private slots:
    void on_pic_open_clicked();

    void on_zoomIn_clicked();
    void on_zoomOut_clicked();

    void on_mir_h_clicked();
    void on_mir_v_clicked();

    void slotAdjustSize();

    void on_angle_valueChanged(double val);

    void on_rotate_plus45_clicked();
    void on_rotate_minus45_clicked();

private:
    void setPixmapToPixmapItem(const QPixmap &pix, qsizetype index = 0);
    Ui::Tab_Widget_Pic *ui;

    QGraphicsScene *_scene;
    QPixmap *_pixmap;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // TAB_WIDGET_PIC_H

#include "tab_widget_pic.h"
#include "qgraphicsitem.h"
#include "ui_tab_widget_pic.h"

#include <QFileDialog>

Tab_Widget_Pic::Tab_Widget_Pic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Widget_Pic),
    _scene(new QGraphicsScene(this)),
    _pixmap(new QPixmap)
{
    ui->setupUi(this);
    ui->angle->setWrapping(true);
    ui->view->setScene(_scene);

    connect(ui->view, &GraphicsView::signalDoubleClick, this, &Tab_Widget_Pic::slotAdjustSize);
}

Tab_Widget_Pic::~Tab_Widget_Pic()
{
    delete ui;
    delete _pixmap;
}

void Tab_Widget_Pic::setPixmap(const QPixmap &pix)
{
    *_pixmap = pix;
    _scene->clear();
    auto item = _scene->addPixmap(pix);
    item->setTransformationMode(Qt::SmoothTransformation);
    slotAdjustSize();
}

void Tab_Widget_Pic::on_pic_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty())
    {
        setPixmap(fileName);

        ui->angle->setEnabled(true);
        ui->save->setEnabled(true);
        ui->rotate_plus45->setEnabled(true);
        ui->rotate_minus45->setEnabled(true);
        ui->mir_h->setEnabled(true);
        ui->mir_v->setEnabled(true);
        ui->zoomIn->setEnabled(true);
        ui->zoomOut->setEnabled(true);
    }
}


void Tab_Widget_Pic::on_zoomIn_clicked()
{
    ui->view->zoom(1.1);
}

void Tab_Widget_Pic::on_zoomOut_clicked()
{
    ui->view->zoom(0.9);
}

void Tab_Widget_Pic::resizeEvent(QResizeEvent *event)
{
    //    qDebug() << "resize event";
    //    if (ui->view->items().size() == 0)
    //        return;

    //    auto item = ui->view->items().at(0);
    //    ui->view->fitInView(item, Qt::KeepAspectRatio);
}

void Tab_Widget_Pic::on_mir_h_clicked()
{
    ui->view->mirrorH();
}

void Tab_Widget_Pic::on_mir_v_clicked()
{
    ui->view->mirrorV();
}

void Tab_Widget_Pic::slotAdjustSize()
{
    if (ui->view->items().size() == 0)
        return;

    auto item = ui->view->items().at(0);
    ui->view->fitInView(item, Qt::KeepAspectRatio);
}

void Tab_Widget_Pic::setPixmapToPixmapItem(const QPixmap &pix, qsizetype index)
{
    if (ui->view->items().size() <= index)
        return;

    auto item = dynamic_cast<QGraphicsPixmapItem *>(ui->view->items().at(index));
    if (item == nullptr)
        return;

    item->setPixmap(*_pixmap);
}


void Tab_Widget_Pic::on_angle_valueChanged(double val)
{
    ui->view->setAngle(val);
}

void Tab_Widget_Pic::on_rotate_plus45_clicked()
{
    float val = ui->angle->value() + 45;
    if (val > 359)
    {
        val = val - 360;
        ui->angle->setValue(val);
        return;
    }

    ui->angle->setValue(ui->angle->value() + 45);
}

void Tab_Widget_Pic::on_rotate_minus45_clicked()
{
    float val = ui->angle->value() - 45;
    if (val < 0)
    {
        val = val + 360;
        ui->angle->setValue(val);
        return;
    }

    ui->angle->setValue(ui->angle->value() - 45);
}


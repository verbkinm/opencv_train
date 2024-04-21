#include "tab_widget_pic.h"
#include "qgraphicsitem.h"
#include "ui_tab_widget_pic.h"

#include <QFileDialog>
#include <QImageReader>

#include "search_type/search_type_cascadeclassifier.h"
#include "search_type/yunet.h"

Tab_Widget_Pic::Tab_Widget_Pic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Widget_Pic),
    _scene(new QGraphicsScene(this)),
    _pixmapItem(new QGraphicsPixmapItem),
    _lastPath(QDir::homePath())
{
    ui->setupUi(this);
    ui->angle->setWrapping(true);
    ui->view->setScene(_scene);
    _pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    _scene->addItem(_pixmapItem);

    connect(ui->view, &GraphicsView::signalDoubleClick, this, &Tab_Widget_Pic::slotAdjustSize);
    connect(ui->view, &GraphicsView::signalMouseMove, this, &Tab_Widget_Pic::signalMouseMove);
}

Tab_Widget_Pic::~Tab_Widget_Pic()
{
    delete ui;
    //    delete _pixmapItem;
}

//void Tab_Widget_Pic::setPixmap(const QPixmap &pix)
//{
////    _pixmapItem->setPixmap(pix);

////    slotAdjustSize();
////    qDebug() << _pixmapItem->boundingRect() << _scene->sceneRect();
//}

void Tab_Widget_Pic::detect(DETECT_TYPE type)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QPixmap pix = _pixmapItem->pixmap();
    pix = pix.transformed(_pixmapItem->transform());
    cv::Mat mat = QPixmapToCvMat(pix);

    if (type == DETECT_TYPE::CASCADECLASSIFIER)
    {
        if (Search_Type_CascadeClassifier::detect(mat))
        {
            ui->angle->setValue(0);
            _pixmapItem->setPixmap(cvMatToQPixmap(mat));
        }
    }
    else if (type == DETECT_TYPE::FaceDetectorYN)
    {
        YuNet model("face_detection_yunet_2023mar.onnx", cv::Size(pix.width(), pix.height()));
        auto faces = model.infer(mat);

        // Print faces
        std::cout << cv::format("%d faces detected:\n", faces.rows) << std::endl;
        for (int i = 0; i < faces.rows; ++i)
        {
            int x1 = static_cast<int>(faces.at<float>(i, 0));
            int y1 = static_cast<int>(faces.at<float>(i, 1));
            int w = static_cast<int>(faces.at<float>(i, 2));
            int h = static_cast<int>(faces.at<float>(i, 3));
            float conf = faces.at<float>(i, 14);
            std::cout << cv::format("%d: x1=%d, y1=%d, w=%d, h=%d, conf=%.4f\n", i, x1, y1, w, h, conf);
        }

        // Draw reults on the input image
        auto res_image = model.visualize(mat, faces);

        ui->angle->setValue(0);
        _pixmapItem->setPixmap(cvMatToQPixmap(res_image));
    }

    QApplication::restoreOverrideCursor();
}

void Tab_Widget_Pic::on_pic_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), _lastPath, tr("Image Files (*.png *.jpg *.bmp)"));
    QFileInfo info(fileName);
    _lastPath = info.path();

    if (!fileName.isEmpty())
    {
        QPixmap pix(fileName);

        //        setPixmap(pix);

        ui->angle->setEnabled(true);
        ui->save->setEnabled(true);
        ui->rotate_plus45->setEnabled(true);
        ui->rotate_minus45->setEnabled(true);
        ui->mir_h->setEnabled(true);
        ui->mir_v->setEnabled(true);
        ui->zoomIn->setEnabled(true);
        ui->zoomOut->setEnabled(true);

        //        ui->angle->setValue(ui->angle->value());
        _pixmapItem->setPixmap(pix);
        slotAdjustSize();
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
    _scene->setSceneRect(_pixmapItem->boundingRect());
    ui->view->fitInView(_pixmapItem, Qt::KeepAspectRatio);

    //!!! Центрирование не становится без следующих строк
    auto val = ui->angle->value();
    ui->view->setAngle(val + 1);
    ui->view->setAngle(val);
}

void Tab_Widget_Pic::setPixmapToPixmapItem(const QPixmap &pix, qsizetype index)
{
    if (ui->view->items().size() <= index)
        return;

    auto item = dynamic_cast<QGraphicsPixmapItem *>(ui->view->items().at(index));
    if (item == nullptr)
        return;

    //    item->setPixmap(*_pixmap);
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


void Tab_Widget_Pic::on_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), _lastPath, tr("Image Files (*.png *.jpg *.bmp)"));

    if (fileName.isEmpty())
        return;

    QFileInfo info(fileName);
    fileName = info.path() + "/" + info.completeBaseName() + ".jpg";
    _lastPath = info.path();

    QPixmap pix = _pixmapItem->pixmap();
    pix = pix.transformed(_pixmapItem->transform());

    pix.save(fileName, "JPG", 100);
    qDebug() << "Файл " << fileName << " сохранён";
}


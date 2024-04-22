#include "tab_widget_cam.h"
#include "qgraphicsitem.h"
#include "ui_tab_widget_cam.h"

#include <QFileDialog>
#include <QImageReader>
#include <QBuffer>
#include <QMessageBox>
#include <QMediaDevices>

#include "search_type/search_type_cascadeclassifier.h"
#include "search_type/yunet.h"
#include "search_type/yolox.h"

Tab_Widget_Cam::Tab_Widget_Cam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Widget_Cam),
    _scene(new QGraphicsScene(this)),
    _pixmapItem(new QGraphicsPixmapItem),
    _lastPath(QDir::currentPath())
{
    ui->setupUi(this);
    ui->angle->setWrapping(true);
    ui->view->setScene(_scene);
    _pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    _pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable);
    _scene->addItem(_pixmapItem);

    endablePanelButtons(true);

    connect(ui->mir_h, &QPushButton::clicked, this, &Tab_Widget_Cam::slotViewTransformedImage);
    connect(ui->mir_v, &QPushButton::clicked, this, &Tab_Widget_Cam::slotViewTransformedImage);
    connect(ui->angle, &QDoubleSpinBox::valueChanged, this, &Tab_Widget_Cam::slotViewTransformedImage);

    connect(ui->view, &GraphicsView::signalDoubleClick, this, &Tab_Widget_Cam::slotAdjustSize);
    connect(ui->view, &GraphicsView::signalMouseMove, this, &Tab_Widget_Cam::signalMouseMove);

    connect(&_imageCapture, &QImageCapture::imageCaptured, this, &Tab_Widget_Cam::processCapturedImage);
    connect(&_imageCapture, &QImageCapture::errorOccurred, this, &Tab_Widget_Cam::errorCapture);
    connect(&_timer, &QTimer::timeout, this, &Tab_Widget_Cam::timerOut, Qt::QueuedConnection);
}

Tab_Widget_Cam::~Tab_Widget_Cam()
{
    delete ui;
    //    delete _pixmapItem;
}

void Tab_Widget_Cam::detect(DETECT_TYPE type)
{
    _detectType = type;
}

void Tab_Widget_Cam::stop()
{
    _camera->stop();
    _timer.stop();
    ui->play_stop->setIcon(QIcon(":/img/play.png"));
    delete _camera;
}

void Tab_Widget_Cam::play()
{
    _camera = new QCamera(QMediaDevices::defaultVideoInput());
    _camera->start();
    _captureSession.setCamera(_camera);
    _captureSession.setImageCapture(&_imageCapture);
    _timer.start(INTERVAL);
    ui->play_stop->setIcon(QIcon(":/img/stop.png"));
}

void Tab_Widget_Cam::on_zoomIn_clicked()
{
    ui->view->zoom(1.1);
}

void Tab_Widget_Cam::on_zoomOut_clicked()
{
    ui->view->zoom(0.9);
}

void Tab_Widget_Cam::keyPressEvent(QKeyEvent *event)
{
    if (event->keyCombination() == QKeyCombination{Qt::ControlModifier, Qt::Key_Q})
        close();

    event->accept();
}

void Tab_Widget_Cam::slotAdjustSize()
{
    _scene->setSceneRect(_pixmapItem->boundingRect());
    ui->view->fitInView(_pixmapItem, Qt::KeepAspectRatio);
}

QImage Tab_Widget_Cam::tansformOriginalImage()
{
    QByteArray byteArray;
    QBuffer buffFile(&byteArray);
    buffFile.open(QIODevice::ReadWrite);

    QImage copyImg = _img;
    QTransform transform;

    qreal x = _img.rect().width() / 2;
    qreal y = _img.rect().height() / 2;
    copyImg = copyImg.transformed(QTransform().translate(x, y).rotate(ui->angle->value()).translate(-x, -y), Qt::SmoothTransformation);
    copyImg.mirror(ui->mir_v->isChecked(), ui->mir_h->isChecked());

    copyImg.save(&buffFile, "JPG", 100);

    return copyImg.fromData(byteArray);
}

void Tab_Widget_Cam::endablePanelButtons(bool state)
{
    ui->play_stop->setEnabled(state);
    ui->screenShoot->setEnabled(state);
    ui->rotate_minus45->setEnabled(state);
    ui->rotate_plus45->setEnabled(state);
    ui->mir_h->setEnabled(state);
    ui->mir_v->setEnabled(state);
    ui->angle->setEnabled(state);
    ui->zoomIn->setEnabled(state);
    ui->zoomOut->setEnabled(state);
}

void Tab_Widget_Cam::detecting(QImage &img)
{
    if (_detectType == DETECT_TYPE::NONE)
        return;

    try
    {
        cv::Mat mat = QImageToCvMat(img);

        if (_detectType == DETECT_TYPE::CASCADECLASSIFIER)
        {
            Search_Type_CascadeClassifier &model = Search_Type_CascadeClassifier::getInstance();
            model.detect(mat, true);
            img = cvMatToQImage(mat);
        }
        else if (_detectType == DETECT_TYPE::FACEDETECTORYN)
        {
            YuNet &model = YuNet::getInstance();
            model.detect(mat, true);
            img = cvMatToQImage(mat);
        }
        else if (_detectType == DETECT_TYPE::YOLOX)
        {
            YoloX &model = YoloX::getInstance();
            model.detect(mat, true);
            img = cvMatToQImage(mat);
        }
    }
    catch(...){}
}

void Tab_Widget_Cam::on_rotate_plus45_clicked()
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

void Tab_Widget_Cam::on_rotate_minus45_clicked()
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

//void Tab_Widget_Cam::on_save_clicked()
//{
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), _lastPath, tr("Image Files (*.png *.jpg *.bmp)"));

//    if (fileName.isEmpty())
//        return;

//    QFileInfo info(fileName);
//    fileName = info.path() + "/" + info.completeBaseName() + ".jpg";
//    _lastPath = info.path();

//    QImage img = tansformOriginalImage();

//    img.save(fileName, "JPG", 100);
//    qDebug() << "Файл " << fileName << " сохранён";
//}

void Tab_Widget_Cam::slotViewTransformedImage()
{
    QImage img = tansformOriginalImage();
    detecting(img);
    _pixmapItem->setPixmap(QPixmap::fromImage(img));
    _scene->setSceneRect(img.rect());
}

void Tab_Widget_Cam::on_screenShoot_clicked()
{
    QImage img = tansformOriginalImage();
    detecting(img);
    emit signalScreenShoot(img);
}

void Tab_Widget_Cam::processCapturedImage(int requestId, const QImage &img)
{
    _timer.stop();

    _img = img;
    slotViewTransformedImage();

    _timer.start(INTERVAL);
}

void Tab_Widget_Cam::timerOut()
{
    if (_imageCapture.isReadyForCapture())
        _imageCapture.capture();
}

void Tab_Widget_Cam::errorCapture(int, QImageCapture::Error err, QString errorStr)
{
    qDebug() << err << ' ' << errorStr;
}

void Tab_Widget_Cam::on_play_stop_clicked()
{
    if (_timer.isActive())
        stop();
    else
        play();
}


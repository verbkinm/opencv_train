#include "tab_widget_pic.h"
#include "qgraphicsitem.h"
#include "ui_tab_widget_pic.h"

#include <QFileDialog>
#include <QImageReader>
#include <QBuffer>

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
    _pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable);
    _scene->addItem(_pixmapItem);

    connect(ui->view, &GraphicsView::signalDoubleClick, this, &Tab_Widget_Pic::slotAdjustSize);
    connect(ui->view, &GraphicsView::signalMouseMove, this, &Tab_Widget_Pic::signalMouseMove);

    connect(ui->mir_h, &QPushButton::clicked, this, &Tab_Widget_Pic::slotViewTransformedImage);
    connect(ui->mir_v, &QPushButton::clicked, this, &Tab_Widget_Pic::slotViewTransformedImage);
    connect(ui->angle, &QDoubleSpinBox::valueChanged, this, &Tab_Widget_Pic::slotViewTransformedImage);
}

Tab_Widget_Pic::~Tab_Widget_Pic()
{
    delete ui;
    //    delete _pixmapItem;
}

void Tab_Widget_Pic::detect(DETECT_TYPE type)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QImage img = tansformOriginalImage();
    cv::Mat mat = QImageToCvMat(img);

    if (type == DETECT_TYPE::CASCADECLASSIFIER)
    {
        Search_Type_CascadeClassifier model;
        int faces = model.detect(mat);
        if (faces)
            newWindowWithDetecetObj(mat, faces);
    }
    else if (type == DETECT_TYPE::FaceDetectorYN)
    {
        YuNet model("face_detection_yunet_2023mar.onnx", cv::Size(img.width(), img.height()));
        auto faces = model.detect(mat);

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

        if (faces.rows)
        {
            // Draw reults on the input image
            auto res_image = model.visualize(mat, faces);

            newWindowWithDetecetObj(res_image, faces.rows);
        }
    }

    QApplication::restoreOverrideCursor();
}

void Tab_Widget_Pic::on_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), _lastPath, tr("Image Files (*.png *.jpg *.bmp)"));
    QFileInfo info(fileName);
    _lastPath = info.path();

    if (!fileName.isEmpty())
    {
        _img.load(fileName);
        slotViewTransformedImage();

        endablePanelButtons(true);

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

void Tab_Widget_Pic::keyPressEvent(QKeyEvent *event)
{
    if (event->keyCombination() == QKeyCombination{Qt::ControlModifier, Qt::Key_Q})
        close();

    event->accept();
}

void Tab_Widget_Pic::slotAdjustSize()
{
    _scene->setSceneRect(_pixmapItem->boundingRect());
    ui->view->fitInView(_pixmapItem, Qt::KeepAspectRatio);
}

QImage Tab_Widget_Pic::tansformOriginalImage()
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

void Tab_Widget_Pic::endablePanelButtons(bool state)
{
    ui->open->setEnabled(state);
    ui->save->setEnabled(state);
    ui->rotate_minus45->setEnabled(state);
    ui->rotate_plus45->setEnabled(state);
    ui->mir_h->setEnabled(state);
    ui->mir_v->setEnabled(state);
    ui->angle->setEnabled(state);
    ui->zoomIn->setEnabled(state);
    ui->zoomOut->setEnabled(state);
}

void Tab_Widget_Pic::newWindowWithDetecetObj(const cv::Mat &mat, int countObject)
{
    Tab_Widget_Pic *window = new Tab_Widget_Pic;
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->setWindowTitle("Detect " + QString::number(countObject) + " object");
    window->_img = cvMatToQImage(mat);
    window->show();

    window->endablePanelButtons(true);
    window->ui->open->setEnabled(false);

    window->slotViewTransformedImage();
    window->slotAdjustSize();
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

    QImage img = tansformOriginalImage();

    img.save(fileName, "JPG", 100);
    qDebug() << "Файл " << fileName << " сохранён";
}

void Tab_Widget_Pic::slotViewTransformedImage()
{
    QImage img = tansformOriginalImage();
    _pixmapItem->setPixmap(QPixmap::fromImage(img));
    _scene->setSceneRect(img.rect());
}


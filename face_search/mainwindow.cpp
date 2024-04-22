#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

#include "search_type/search_type_cascadeclassifier.h"
#include "search_type/yolox.h"
#include "search_type/yunet.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    _lastPath(QDir::currentPath())
{
    ui->setupUi(this);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::slotTabChange);
    connect(ui->tab_pic, &Tab_Widget_Pic::signalMouseMove, this, &MainWindow::slotMouseMove);
    connect(ui->tab_cam, &Tab_Widget_Cam::signalMouseMove, this, &MainWindow::slotMouseMove);

    connect(ui->tab_cam, &Tab_Widget_Cam::signalScreenShoot, this, &MainWindow::slotCameraScreenShoot);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_triggered()
{
    exit(0);
}

void MainWindow::on_detect_clicked()
{
    DETECT_TYPE type = static_cast<DETECT_TYPE>(ui->detectionType->currentIndex());

    int currTab = ui->tabWidget->currentIndex();
    if (currTab == TAB_NAME::TAB_PICTURE)
        ui->tab_pic->detect(type);
    else if(currTab == TAB_NAME::TAB_VIDEO)
        ;
    //    else if(currTab == TAB_NAME::TAB_CAMERA)
    //        ui->tab_cam->detect(type);
}

void MainWindow::slotMouseMove(QPointF point)
{
    statusBar()->showMessage("x: " + QString::number(point.x()) + ", y: " + QString::number(point.y()));
}

void MainWindow::on_modelPathSelect_clicked()
{
    if (ui->detectionType->currentIndex() == static_cast<int>(DETECT_TYPE::CASCADECLASSIFIER))
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open model"), _lastPath, tr("xml Files (*.xml)"));

        if (fileName.isEmpty())
            return;

        QFileInfo info(fileName);
        _lastPath = info.path();

        Search_Type_CascadeClassifier &instance = Search_Type_CascadeClassifier::getInstance();
        instance.setModelPath(fileName.toStdString());
    }
    else if (ui->detectionType->currentIndex() == static_cast<int>(DETECT_TYPE::FACEDETECTORYN))
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open model"), _lastPath, tr("onnx Files (*.onnx)"));

        if (fileName.isEmpty())
            return;

        QFileInfo info(fileName);
        _lastPath = info.path();

        YuNet &instance = YuNet::getInstance();
        instance.setModelPath(fileName.toStdString());
    }
    else if (ui->detectionType->currentIndex() == static_cast<int>(DETECT_TYPE::YOLOX))
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open model"), _lastPath, tr("onnx Files (*.onnx)"));

        if (fileName.isEmpty())
            return;

        QFileInfo info(fileName);
        _lastPath = info.path();

        YoloX &instance = YoloX::getInstance();
        instance.setModelPath(fileName.toStdString());
    }
}

void MainWindow::slotCameraScreenShoot(QImage img)
{
    ui->tabWidget->setCurrentIndex(0);
    ui->tab_pic->setImage(img);
}

void MainWindow::slotTabChange(int index)
{
    if (index == TAB_NAME::TAB_CAMERA)
        ui->detect->setCheckable(true);
    else
    {
        ui->detect->setChecked(false);
        ui->detect->setCheckable(false);
    }
}

void MainWindow::on_detect_toggled(bool checked)
{
    DETECT_TYPE type = static_cast<DETECT_TYPE>(ui->detectionType->currentIndex());

    qDebug() << "toogle" << static_cast<int>(type) << checked;

    if (checked)
        ui->tab_cam->detect(type);
    else
        ui->tab_cam->detect(DETECT_TYPE::NONE);

}


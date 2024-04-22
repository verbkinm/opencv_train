#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

#include "search_type/search_type_cascadeclassifier.h"
#include "search_type/yunet.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    _lastPath(QDir::currentPath())
{
    ui->setupUi(this);

    connect(ui->tab_pic, &Tab_Widget_Pic::signalMouseMove, this, &MainWindow::slotMouseMove);
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
    ui->tab_pic->detect(type);
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
}


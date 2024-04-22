#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "search_type/search_type_cascadeclassifier.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

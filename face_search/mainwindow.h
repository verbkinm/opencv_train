#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_triggered();

    void on_detect_clicked();

    void slotMouseMove(QPointF point);

    void on_modelPathSelect_clicked();

private:
    Ui::MainWindow *ui;

    QString _lastPath;
};
#endif // MAINWINDOW_H

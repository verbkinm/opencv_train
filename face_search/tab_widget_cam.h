#ifndef TAB_WIDGET_CAM_H
#define TAB_WIDGET_CAM_H

#include <QWidget>
#include <QGraphicsScene>
#include "Converter_Qt_Opencv.h"
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QTimer>

namespace Ui {
class Tab_Widget_Cam;
}

class Tab_Widget_Cam : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Widget_Cam(QWidget *parent = nullptr);
    ~Tab_Widget_Cam();

    void detect(DETECT_TYPE type);
    void stop();
    void play();

private slots:
//    void on_open_clicked();

    void on_zoomIn_clicked();
    void on_zoomOut_clicked();

    void slotAdjustSize();

    void on_rotate_plus45_clicked();
    void on_rotate_minus45_clicked();

//    void on_save_clicked();

    void slotViewTransformedImage();

    void on_screenShoot_clicked();

    void processCapturedImage(int requestId, const QImage &img);
    void timerOut();
    void errorCapture(int, QImageCapture::Error err, QString errorStr);

    void on_play_stop_clicked();

signals:
    void signalMouseMove(QPointF point);
    void signalScreenShoot(QImage img);

private:
    QImage tansformOriginalImage();
    void endablePanelButtons(bool state);
    void detecting(QImage &img);
//    void newWindowWithDetecetObj(const cv::Mat &mat, int countObject);


    Ui::Tab_Widget_Cam *ui;

    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_pixmapItem;

    QString _lastPath;
    QImage _img;

    QCamera *_camera;
    QMediaCaptureSession _captureSession;
    QImageCapture _imageCapture;
    QTimer _timer;
    static constexpr int INTERVAL = 10;

    DETECT_TYPE _detectType = DETECT_TYPE::NONE;

//    cv::Mat _mat;

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // TAB_WIDGET_CAM_H

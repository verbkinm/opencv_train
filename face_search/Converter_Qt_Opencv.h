#ifndef CONVERTER_QT_OPENCV_H
#define CONVERTER_QT_OPENCV_H

#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>

enum class DETECT_TYPE {
    CASCADECLASSIFIER,
    FACEDETECTORYN,
    YOLOX,
    NONE
};

QImage cvMatToQImage(const cv::Mat &image);
QPixmap cvMatToQPixmap(const cv::Mat &image);

cv::Mat QImageToCvMat(QImage &inImage, bool inCloneImageData = true );
cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );

#endif // CONVERTER_QT_OPENCV_H

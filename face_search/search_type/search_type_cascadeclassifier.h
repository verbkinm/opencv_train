#ifndef SEARCH_TYPE_CASCADECLASSIFIER_H
#define SEARCH_TYPE_CASCADECLASSIFIER_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>

class Search_Type_CascadeClassifier
{
public:
    Search_Type_CascadeClassifier() = delete;
    static void loadDefaultXml();
    static int detect(cv::Mat &img);
};

#endif // SEARCH_TYPE_CASCADECLASSIFIER_H

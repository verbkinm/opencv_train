#ifndef SEARCH_TYPE_CASCADECLASSIFIER_H
#define SEARCH_TYPE_CASCADECLASSIFIER_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>

class Search_Type_CascadeClassifier
{
public:
    Search_Type_CascadeClassifier();
    void loadXml(const std::string &xml);
    int detect(cv::Mat &img);

private:
    cv::CascadeClassifier _face_cascade;
    bool _loadState;
    std::string _xml;
};

#endif // SEARCH_TYPE_CASCADECLASSIFIER_H

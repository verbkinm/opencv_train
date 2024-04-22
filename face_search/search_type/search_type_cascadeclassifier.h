#ifndef SEARCH_TYPE_CASCADECLASSIFIER_H
#define SEARCH_TYPE_CASCADECLASSIFIER_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>

class Search_Type_CascadeClassifier
{
public:
    static Search_Type_CascadeClassifier &getInstance();

    void setModelPath(const std::string &modelPath);
    int detect(cv::Mat &img, bool fps = false);

    Search_Type_CascadeClassifier(const Search_Type_CascadeClassifier&) = delete;
    Search_Type_CascadeClassifier& operator=(const Search_Type_CascadeClassifier&) = delete;

private:
    Search_Type_CascadeClassifier();

    cv::CascadeClassifier _cascadeClassifier;
    bool _loadState;
    std::string _xml;

    static Search_Type_CascadeClassifier *instance;
};

#endif // SEARCH_TYPE_CASCADECLASSIFIER_H

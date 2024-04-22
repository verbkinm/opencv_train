#include "search_type_cascadeclassifier.h"

#include <iostream>

Search_Type_CascadeClassifier::Search_Type_CascadeClassifier() : _loadState(false),
    _xml("haarcascade_frontalface_alt2.xml")
{
    loadXml(_xml);
}

void Search_Type_CascadeClassifier::loadXml(const std::string &xml)
{
    _xml = xml;
    if(!_face_cascade.load(xml))
    {
        std::cerr << "Load xml failed." << std::endl;
        _loadState = false;
        return;
    }

    std::cout << "Load xml succeed." << std::endl;
    _loadState = true;
}

int Search_Type_CascadeClassifier::detect(cv::Mat &img)
{
    if (img.empty())
    {
        std::cerr << "Image emty" << std::endl;
        return 0;
    }

    if (_loadState == false)
    {
        std::cerr << "Load xml failed." << std::endl;
        return 0;
    }

    std::vector<cv::Rect> objects;
    cv::Mat img_gary;


    cv::cvtColor(img, img_gary, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(img_gary, img_gary);

    _face_cascade.detectMultiScale(img_gary, objects, 1.1, 3, 0, cv::Size(50, 50));

    for(size_t i = 0; i < objects.size(); i++)
        cv::rectangle(img, objects[i], cv::Scalar(0, 255, 0), 10);

    return objects.size();
}

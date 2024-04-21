#ifndef YUNET_H
#define YUNET_H

#include "opencv2/opencv.hpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>

class YuNet
{
public:
    YuNet(const std::string& model_path,
          const cv::Size& input_size = cv::Size(320, 320),
          float conf_threshold = 0.6f,
          float nms_threshold = 0.3f,
          int top_k = 5000,
          int backend_id = 0,
          int target_id = 0);

    void setInputSize(const cv::Size& input_size);
    cv::Mat infer(const cv::Mat image);
    cv::Mat visualize(const cv::Mat& image, const cv::Mat& faces, float fps = -1.f);

private:
    cv::Ptr<cv::FaceDetectorYN> model;

    std::string model_path_;
    cv::Size input_size_;
    float conf_threshold_;
    float nms_threshold_;
    int top_k_;
    int backend_id_;
    int target_id_;
};

#endif // YUNET_H

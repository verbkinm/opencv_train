#ifndef YUNET_H
#define YUNET_H

#include "opencv2/opencv.hpp"

class YuNet
{
public:
    static YuNet &getInstance();

    void setModelPath(const std::string &modelPath);
    void setInputSize(const cv::Size& input_size);
    int detect(cv::Mat &image, bool fps = false);

    YuNet(const YuNet&) = delete;
    YuNet& operator=(const YuNet&) = delete;

private:
    YuNet(const std::string& model_path,
          const cv::Size& input_size = cv::Size(320, 320),
          float conf_threshold = 0.6f,
          float nms_threshold = 0.3f,
          int top_k = 5000,
          int backend_id = 0,
          int target_id = 0);

    cv::Mat visualize(const cv::Mat& image, const cv::Mat& faces, float fps = -1.f);

    cv::Ptr<cv::FaceDetectorYN> model;

    std::string model_path_;
    cv::Size input_size_;
    float conf_threshold_;
    float nms_threshold_;
    int top_k_;
    int backend_id_;
    int target_id_;

    bool _loadState;

     static YuNet *instance;
};

#endif // YUNET_H

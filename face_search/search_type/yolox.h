#ifndef YOLOX_H
#define YOLOX_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace dnn;


class YoloX {
private:
    Net net;
    string modelPath;
    Size inputSize;
    float confThreshold;
    float nmsThreshold;
    float objThreshold;
    dnn::Backend backendId;
    dnn::Target targetId;
    int num_classes;
    vector<int> strides;
    Mat expandedStrides;
    Mat grids;

    static YoloX *instance;

    YoloX(string modPath, float confThresh = 0.35, float nmsThresh = 0.5, float objThresh = 0.5, dnn::Backend bId = DNN_BACKEND_DEFAULT, dnn::Target tId = DNN_TARGET_CPU);
    Mat preprocess(Mat img);
    Mat infer(Mat srcimg);
    Mat postprocess(Mat outputs);

    void generateAnchors();

    pair<Mat, double> letterBox(Mat srcimg, Size targetSize = Size(640, 640));
    Mat unLetterBox(Mat bbox, double letterboxScale);
    Mat visualize(Mat dets, Mat srcimg, double letterbox_scale, double fps = -1);

public:
    static YoloX &getInstance();
    void setModelPath(const std::string &modelPath);

    YoloX(const YoloX&) = delete;
    YoloX& operator=(const YoloX&) = delete;

    int detect(cv::Mat &img, bool fps = false);
};

#endif // YOLOX_H

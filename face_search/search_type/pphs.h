#ifndef PPHS_H
#define PPHS_H

#include "opencv2/opencv.hpp"

#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace cv;
using namespace dnn;

class PPHS
{
private:
    Net model;
    string modelPath;

    Scalar imageMean = Scalar(0.5,0.5,0.5);
    Scalar imageStd = Scalar(0.5,0.5,0.5);
    Size modelInputSize = Size(192, 192);
    Size currentSize;

    const String inputNames = "x";
    const String outputNames = "save_infer_model/scale_0.tmp_1";

    int backend_id;
    int target_id;

    static PPHS *instance;

    PPHS(const string& modelPath,
         int backend_id = 0,
         int target_id = 0);

    Mat preprocess(const Mat image);
    Mat infer(const Mat image);
    Mat postprocess(Mat image);
    vector<uint8_t> getColorMapList(int num_classes);
    Mat visualize(const Mat& image, const Mat& result, float fps = -1.f, float weight = 0.4);

public:
    static PPHS &getInstance();

    PPHS(const PPHS&) = delete;
    PPHS& operator=(const PPHS&) = delete;

    int detect(cv::Mat &img, bool fps = false);
    void setModelPath(const std::string &modelPath);
};


#endif // PPHS_H

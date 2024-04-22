#include "yolox.h"

YoloX *YoloX::instance = nullptr;

vector< pair<dnn::Backend, dnn::Target> > backendTargetPairs = {
    std::make_pair<dnn::Backend, dnn::Target>(dnn::DNN_BACKEND_OPENCV, dnn::DNN_TARGET_CPU),
    std::make_pair<dnn::Backend, dnn::Target>(dnn::DNN_BACKEND_CUDA, dnn::DNN_TARGET_CUDA),
    std::make_pair<dnn::Backend, dnn::Target>(dnn::DNN_BACKEND_CUDA, dnn::DNN_TARGET_CUDA_FP16),
    std::make_pair<dnn::Backend, dnn::Target>(dnn::DNN_BACKEND_TIMVX, dnn::DNN_TARGET_NPU),
    std::make_pair<dnn::Backend, dnn::Target>(dnn::DNN_BACKEND_CANN, dnn::DNN_TARGET_NPU) };

vector<string> labelYolox = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus",
    "train", "truck", "boat", "traffic light", "fire hydrant",
    "stop sign", "parking meter", "bench", "bird", "cat", "dog",
    "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
    "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat",
    "baseball glove", "skateboard", "surfboard", "tennis racket",
    "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
    "banana", "apple", "sandwich", "orange", "broccoli", "carrot",
    "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop",
    "mouse", "remote", "keyboard", "cell phone", "microwave",
    "oven", "toaster", "sink", "refrigerator", "book", "clock",
    "vase", "scissors", "teddy bear", "hair drier", "toothbrush" };

YoloX::YoloX(string modPath, float confThresh, float nmsThresh, float objThresh, Backend bId, Target tId) :
    modelPath(modPath), confThreshold(confThresh),
    nmsThreshold(nmsThresh), objThreshold(objThresh),
    backendId(bId), targetId(tId)
{
    this->num_classes = int(labelYolox.size());
    this->net = readNet(modelPath);
    this->inputSize = Size(640, 640);
    this->strides = vector<int>{ 8, 16, 32 };
    this->net.setPreferableBackend(this->backendId);
    this->net.setPreferableTarget(this->targetId);
    this->generateAnchors();
}

int YoloX::detect(Mat &img, bool fps)
{
    Mat inputBlob;
    double letterboxScale;
    int nbInference = 0;

    if (img.empty())
    {
        cout << "Frame is empty" << endl;
        waitKey();
        return 0;
    }
    pair<Mat, double> w = letterBox(img);//, cv::Size(img.width(), img.height()));
    inputBlob = get<0>(w);
    letterboxScale  = get<1>(w);

    auto tick_meter = cv::TickMeter();
    tick_meter.start();
    Mat predictions = infer(inputBlob);
    tick_meter.stop();

    if (fps)
        img = visualize(predictions, img, letterboxScale, (float)tick_meter.getFPS());
    else
        img = visualize(predictions, img, letterboxScale);

    return predictions.rows;
}

YoloX &YoloX::getInstance()
{
    if (!instance) {
        instance = new YoloX("object_detection_yolox_2022nov.onnx");
    }
    return *instance;
}

void YoloX::setModelPath(const string &modelPath)
{
    net = readNet(modelPath);
}

Mat YoloX::preprocess(Mat img)
{
    Mat blob;
    Image2BlobParams paramYolox;
    paramYolox.datalayout = DNN_LAYOUT_NCHW;
    paramYolox.ddepth = CV_32F;
    paramYolox.mean = Scalar::all(0);
    paramYolox.scalefactor = Scalar::all(1);
    paramYolox.size = Size(img.cols, img.rows);
    paramYolox.swapRB = true;

    blob = blobFromImageWithParams(img, paramYolox);
    return blob;
}

Mat YoloX::infer(Mat srcimg)
{
    Mat inputBlob = this->preprocess(srcimg);

    this->net.setInput(inputBlob);
    vector<Mat> outs;
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());

    Mat predictions = this->postprocess(outs[0]);
    return predictions;
}

Mat YoloX::postprocess(Mat outputs)
{
    Mat dets = outputs.reshape(0,outputs.size[1]);
    Mat col01;
    add(dets.colRange(0, 2), this->grids, col01);
    Mat col23;
    exp(dets.colRange(2, 4), col23);
    vector<Mat> col = { col01, col23 };
    Mat boxes;
    hconcat(col, boxes);
    float* ptr = this->expandedStrides.ptr<float>(0);
    for (int r = 0; r < boxes.rows; r++, ptr++)
    {
        boxes.rowRange(r, r + 1) = *ptr * boxes.rowRange(r, r + 1);
    }
    // get boxes
    Mat boxes_xyxy(boxes.rows, boxes.cols, CV_32FC1, Scalar(1));
    Mat scores = dets.colRange(5, dets.cols).clone();
    vector<float> maxScores(dets.rows);
    vector<int> maxScoreIdx(dets.rows);
    vector<Rect2d> boxesXYXY(dets.rows);

    for (int r = 0; r < boxes_xyxy.rows; r++, ptr++)
    {
        boxes_xyxy.at<float>(r, 0) = boxes.at<float>(r, 0) - boxes.at<float>(r, 2) / 2.f;
        boxes_xyxy.at<float>(r, 1) = boxes.at<float>(r, 1) - boxes.at<float>(r, 3) / 2.f;
        boxes_xyxy.at<float>(r, 2) = boxes.at<float>(r, 0) + boxes.at<float>(r, 2) / 2.f;
        boxes_xyxy.at<float>(r, 3) = boxes.at<float>(r, 1) + boxes.at<float>(r, 3) / 2.f;
        // get scores and class indices
        scores.rowRange(r, r + 1) = scores.rowRange(r, r + 1) * dets.at<float>(r, 4);
        double minVal, maxVal;
        Point maxIdx;
        minMaxLoc(scores.rowRange(r, r+1), &minVal, &maxVal, nullptr, &maxIdx);
        maxScoreIdx[r] = maxIdx.x;
        maxScores[r] = float(maxVal);
        boxesXYXY[r].x = boxes_xyxy.at<float>(r, 0);
        boxesXYXY[r].y = boxes_xyxy.at<float>(r, 1);
        boxesXYXY[r].width = boxes_xyxy.at<float>(r, 2);
        boxesXYXY[r].height = boxes_xyxy.at<float>(r, 3);
    }

    vector<int> keep;
    NMSBoxesBatched(boxesXYXY, maxScores, maxScoreIdx, this->confThreshold, this->nmsThreshold, keep);
    Mat candidates(int(keep.size()), 6, CV_32FC1);
    int row = 0;
    for (auto idx : keep)
    {
        boxes_xyxy.rowRange(idx, idx + 1).copyTo(candidates(Rect(0, row, 4, 1)));
        candidates.at<float>(row, 4) = maxScores[idx];
        candidates.at<float>(row, 5) = float(maxScoreIdx[idx]);
        row++;
    }
    if (keep.size() == 0)
        return Mat();
    return candidates;

}

void YoloX::generateAnchors()
{
    vector< tuple<int, int, int> > nb;
    int total = 0;

    for (auto v : this->strides)
    {
        int w = this->inputSize.width / v;
        int h = this->inputSize.height / v;
        nb.push_back(tuple<int, int, int>(w * h, w, v));
        total += w * h;
    }
    this->grids = Mat(total, 2, CV_32FC1);
    this->expandedStrides = Mat(total, 1, CV_32FC1);
    float* ptrGrids = this->grids.ptr<float>(0);
    float* ptrStrides = this->expandedStrides.ptr<float>(0);
    int pos = 0;
    for (auto le : nb)
    {
        int r = get<1>(le);
        for (int i = 0; i < get<0>(le); i++, pos++)
        {
            *ptrGrids++ = float(i % r);
            *ptrGrids++ = float(i / r);
            *ptrStrides++ = float((get<2>(le)));
        }
    }
}

pair<Mat, double> YoloX::letterBox(Mat srcimg, Size targetSize)
{
    Mat paddedImg(targetSize.height, targetSize.width, CV_32FC3, Scalar::all(114.0));
    Mat resizeImg;

    double ratio = min(targetSize.height / double(srcimg.rows), targetSize.width / double(srcimg.cols));
    resize(srcimg, resizeImg, Size(int(srcimg.cols * ratio), int(srcimg.rows * ratio)), INTER_LINEAR);
    resizeImg.copyTo(paddedImg(Rect(0, 0, int(srcimg.cols * ratio), int(srcimg.rows * ratio))));
    return pair<Mat, double>(paddedImg, ratio);
}

Mat YoloX::unLetterBox(Mat bbox, double letterboxScale)
{
    return bbox / letterboxScale;
}

Mat YoloX::visualize(Mat dets, Mat srcimg, double letterbox_scale, double fps)
{
    Mat resImg = srcimg.clone();

    if (fps > 0)
        putText(resImg, format("FPS: %.2f", fps), Size(10, 25), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

    for (int row = 0; row < dets.rows; row++)
    {
        Mat boxF = unLetterBox(dets(Rect(0, row, 4, 1)), letterbox_scale);
        Mat box;
        boxF.convertTo(box, CV_32S);
        float score = dets.at<float>(row, 4);
        int clsId = int(dets.at<float>(row, 5));

        int x0 = box.at<int>(0, 0);
        int y0 = box.at<int>(0, 1);
        int x1 = box.at<int>(0, 2);
        int y1 = box.at<int>(0, 3);

        string text = format("%s : %f", labelYolox[clsId].c_str(), score * 100);
        int font = FONT_HERSHEY_SIMPLEX;
        int baseLine = 0;
        Size txtSize = getTextSize(text, font, 0.4, 1, &baseLine);
        rectangle(resImg, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 0), 2);
        rectangle(resImg, Point(x0, y0 + 1), Point(x0 + txtSize.width + 1, y0 + int(1.5 * txtSize.height)), Scalar(255, 255, 255), -1);
        putText(resImg, text, Point(x0, y0 + txtSize.height), font, 0.4, Scalar(0, 0, 0), 1);
    }

    return resImg;
}

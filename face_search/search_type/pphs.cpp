#include "pphs.h"

PPHS *PPHS::instance = nullptr;


std::vector<std::pair<int, int>> backend_target_pairs = {
    {DNN_BACKEND_OPENCV, DNN_TARGET_CPU},
    {DNN_BACKEND_CUDA, DNN_TARGET_CUDA},
    {DNN_BACKEND_CUDA, DNN_TARGET_CUDA_FP16},
    {DNN_BACKEND_TIMVX, DNN_TARGET_NPU},
    {DNN_BACKEND_CANN, DNN_TARGET_NPU}
};

PPHS &PPHS::getInstance()
{
    if (!instance) {
        instance = new PPHS("human_segmentation_pphumanseg_2023mar.onnx");
    }
    return *instance;
}

PPHS::PPHS(const string &modelPath, int backend_id, int target_id)
    : modelPath(modelPath), backend_id(backend_id), target_id(target_id)
{
    this->model = readNet(modelPath);
    this->model.setPreferableBackend(backend_id);
    this->model.setPreferableTarget(target_id);
}

int PPHS::detect(Mat &img, bool fps)
{
    Mat result;
    TickMeter tm;

    tm.start();
    result = infer(img);
    tm.stop();

    img = visualize(img, result, tm.getFPS());

    return -1;
}

Mat PPHS::preprocess(const Mat image)
{
    this->currentSize = image.size();
    Mat preprocessed = Mat::zeros(this->modelInputSize, image.type());
    resize(image, preprocessed, this->modelInputSize);

    // image normalization
    preprocessed.convertTo(preprocessed, CV_32F, 1.0 / 255.0);
    preprocessed -= imageMean;
    preprocessed /= imageStd;

    return blobFromImage(preprocessed);;
}

Mat PPHS::infer(const Mat image)
{
    Mat inputBlob = preprocess(image);

    this->model.setInput(inputBlob, this->inputNames);
    Mat outputBlob = this->model.forward(this->outputNames);

    return postprocess(outputBlob);
}

Mat PPHS::postprocess(Mat image)
{
    reduceArgMax(image,image,1);
    image = image.reshape(1,image.size[2]);
    image.convertTo(image, CV_32F);
    resize(image, image, this->currentSize, 0, 0, INTER_LINEAR);
    image.convertTo(image, CV_8U);

    return image;
}

vector<uint8_t> PPHS::getColorMapList(int num_classes) {
    num_classes += 1;

    vector<uint8_t> cm(num_classes*3, 0);

    int lab, j;

    for (int i = 0; i < num_classes; ++i) {
        lab = i;
        j = 0;

        while(lab){
            cm[i] |= (((lab >> 0) & 1) << (7 - j));
            cm[i+num_classes] |= (((lab >> 1) & 1) << (7 - j));
            cm[i+2*num_classes] |= (((lab >> 2) & 1) << (7 - j));
            ++j;
            lab >>= 3;
        }

    }

    cm.erase(cm.begin(), cm.begin()+3);

    return cm;
}

Mat PPHS::visualize(const Mat &image, const Mat &result, float fps, float weight)
{
    const Scalar& text_color = Scalar(0, 255, 0);
    Mat output_image = image.clone();

    vector<uint8_t> color_map = getColorMapList(256);

    Mat cmm(color_map);

    cmm = cmm.reshape(1,{3,256});

    if (fps >= 0)
    {
        putText(output_image, format("FPS: %.2f", fps), Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    Mat c1, c2, c3;

    LUT(result, cmm.row(0), c1);
    LUT(result, cmm.row(1), c2);
    LUT(result, cmm.row(2), c3);

    Mat pseudo_img;
    merge(std::vector<Mat>{c1,c2,c3}, pseudo_img);

    addWeighted(output_image, weight, pseudo_img, 1 - weight, 0, output_image);

    return output_image;
}

#include "yunet.h"
#include <fstream>

YuNet *YuNet::instance = nullptr;

YuNet::YuNet(const std::string &model_path, const cv::Size &input_size, float conf_threshold, float nms_threshold, int top_k, int backend_id, int target_id)
    : model_path_(model_path), input_size_(input_size),
    conf_threshold_(conf_threshold), nms_threshold_(nms_threshold),
    top_k_(top_k), backend_id_(backend_id), target_id_(target_id),
    _loadState(false)
{
    std::ifstream file(model_path_);
    if (file.is_open())
    {
        model = cv::FaceDetectorYN::create(model_path_, "", input_size_, conf_threshold_, nms_threshold_, top_k_, backend_id_, target_id_);
        std::cout << "Load " << model_path_ << " succeed." << std::endl;
        _loadState = true;
    }
    else
        std::cerr << "Load " << model_path_ << " failed." << std::endl;
}

YuNet &YuNet::getInstance()
{
    if (!instance)
        instance = new YuNet("face_detection_yunet_2023mar.onnx", cv::Size());

    return *instance;
}

void YuNet::setModelPath(const std::string &modelPath)
{
    try
    {
        model_path_ = modelPath;
        model = cv::FaceDetectorYN::create(model_path_, "", input_size_, conf_threshold_, nms_threshold_, top_k_, backend_id_, target_id_);
        std::cout << "Load " << model_path_ << " succeed." << std::endl;
        _loadState = true;
    }
    catch(...)
    {
        std::cerr << "Load " << model_path_ << " failed." << std::endl;
        _loadState = false;
    }
}

void YuNet::setInputSize(const cv::Size &input_size)
{
    if (_loadState == false)
    {
        std::cerr << "Load " << model_path_ << " failed." << std::endl;
        return;
    }

    input_size_ = input_size;
    model->setInputSize(input_size_);
}

int YuNet::detect(cv::Mat &image, bool fps)
{
    if (_loadState == false)
    {
        std::cerr << "Load " << model_path_ << " failed." << std::endl;
        return {};
    }

    setInputSize({image.cols, image.rows});
    cv::Mat res;

    auto tick_meter = cv::TickMeter();
    tick_meter.start();
    model->detect(image, res);
    tick_meter.stop();

    if (res.rows) // res.rows
    {
        if (fps)
            image = visualize(image, res, (float)tick_meter.getFPS());
        else
            image = visualize(image, res);

        return res.rows;
    }

    return 0;
}

cv::Mat YuNet::visualize(const cv::Mat &image, const cv::Mat &faces, float fps)
{
    static cv::Scalar box_color{0, 255, 0};
    static std::vector<cv::Scalar> landmark_color{
        cv::Scalar(255,   0,   0), // right eye
        cv::Scalar(  0,   0, 255), // left eye
        cv::Scalar(  0, 255,   0), // nose tip
        cv::Scalar(255,   0, 255), // right mouth corner
        cv::Scalar(  0, 255, 255)  // left mouth corner
    };
    static cv::Scalar text_color{0, 255, 0};

    auto output_image = image.clone();

    if (fps >= 0)
    {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (int i = 0; i < faces.rows; ++i)
    {
        // Draw bounding boxes
        int x1 = static_cast<int>(faces.at<float>(i, 0));
        int y1 = static_cast<int>(faces.at<float>(i, 1));
        int w = static_cast<int>(faces.at<float>(i, 2));
        int h = static_cast<int>(faces.at<float>(i, 3));
        cv::rectangle(output_image, cv::Rect(x1, y1, w, h), box_color, 2);

        // Confidence as text
        float conf = faces.at<float>(i, 14);
        cv::putText(output_image, cv::format("%.4f", conf), cv::Point(x1, y1+12), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color);

        // Draw landmarks
        for (int j = 0; j < landmark_color.size(); ++j)
        {
            int x = static_cast<int>(faces.at<float>(i, 2*j+4)), y = static_cast<int>(faces.at<float>(i, 2*j+5));
            cv::circle(output_image, cv::Point(x, y), 2, landmark_color[j], 2);
        }
    }
    return output_image;
}

#include "opencv2/face/facerec.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

using namespace cv;
namespace fs = std::filesystem;

// The function is used to read the image files in the folder and extract the face feature vector
void extractFaceFeatures(const std::string &folderPath, std::vector<cv::Mat> &images, std::vector<int> &labels)
{
    for (const auto & personDir : fs::directory_iterator(folderPath))
    {
        if (!fs::is_directory(personDir)) continue;
        int label = std::stoi(personDir. path(). filename(). string());

        for (const auto &imgPath : fs::directory_iterator(personDir))
        {
            cv::Mat image = cv::imread(imgPath.path().string(), cv::IMREAD_GRAYSCALE);
            if (image.empty())
                continue;

            images. push_back(image);
            labels. push_back(label);
        }
    }
}

int main()
{
    std::string dataFolderPath = "Img";
    std::vector<cv::Mat> images;
    std::vector<int> labels;

    extractFaceFeatures(dataFolderPath, images, labels);

    // Create and train a face recognition model (using LBPH algorithm)
    Ptr<face::LBPHFaceRecognizer> model1 = face::LBPHFaceRecognizer::create();
    model1->train(images, labels);

    Ptr<face::EigenFaceRecognizer> model2 = face::EigenFaceRecognizer::create();
    model2->train(images, labels);


//    Ptr<face::FisherFaceRecognizer> model3 = face::FisherFaceRecognizer::create();
//    model3->train(images, labels);


    // save the model to disk
    model1->save("LBPHFaceRecognizerModel.xml");
    model2->save("EigenFaceRecognizerModel.xml");
//    model3->save("FisherFaceRecognizer.xml");

    std::cout << "The model training is completed and saved successfully!" << std::endl;

    return 0;
}

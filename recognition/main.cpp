#include "opencv2/face/facerec.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main()
{
    // Load the trained face recognition model
    Ptr<face::LBPHFaceRecognizer> model1 = face::LBPHFaceRecognizer::create();
    model1->read("LBPHFaceRecognizerModel.xml");

    Ptr<face::EigenFaceRecognizer> model2 = face::EigenFaceRecognizer::create();
    model2->read("EigenFaceRecognizerModel.xml");

    // Initialize the camera
    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        std::cerr << "Could not open the camera!" << std::endl;
        return -1;
    }

    // Create a face detector
    CascadeClassifier faceCascade;
    faceCascade.load("haarcascade_frontalface_alt2.xml");

    while (true)
    {
        Mat frame;
        capture >> frame;
        if (frame.empty()) break;

        // convert to grayscale image
        Mat grayFrame;
        cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
        equalizeHist(grayFrame, grayFrame);

        // Face Detection
        std::vector<Rect> faces;
        faceCascade.detectMultiScale(grayFrame, faces, 1.1, 3, 0, Size(50, 50));

        for (const Rect &face : faces)
        {
            // face recognition
            Mat faceImage = grayFrame(face);
            Mat face_resize;
            resize(faceImage, face_resize, Size(256, 256));
            equalizeHist(face_resize, face_resize);

            int label;
            double confidence1, confidence2;
            model1->predict(face_resize, label, confidence1);
            model2->predict(face_resize, label, confidence2);

            // display recognition result
            std::string labelText;
            if (confidence1 < 41.0 /*&& confidence2 > 4000*/) // Set a threshold to judge the recognition result
            {
                labelText = "Mikhail";
                // draw a rectangle on the image
                rectangle(frame, face, Scalar(0, 255, 0), 2);
            }
            else
            {
                labelText = "Unknown";
                rectangle(frame, face, Scalar(0, 0, 255), 2);
            }

            std::string labelTextLBPH = "LBPH: " + std::to_string((int)confidence1);
            std::string labelTextEigen = "Eigen: " + std::to_string((int)confidence2);

            Point labelPosition(face.x, face.y - 10);
            Point labelLBPHPostition(face.x, face.y + face.height + 20);
            Point labelEigenPostition(face.x, face.y + face.height + 40);

            putText(frame, labelText, labelPosition, FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255, 0, 0), 2);
            putText(frame, labelTextLBPH, labelLBPHPostition, FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
            putText(frame, labelTextEigen, labelEigenPostition, FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
        }

        // display the video stream
        imshow("Face Recognition", frame);
        //        imshow("Face Recognition gray", grayFrame);

        // Press the ESC key to exit
        if (waitKey(10) == 27)
            break;
    }

    return 0;
}

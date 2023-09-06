#include <grpcpp/grpcpp.h>
#include "video_service.grpc.pb.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

 using video :: VideoService;
using video :: Empty;
using video :: Frame;

#ifdef _DEBUG
#pragma comment(lib, "opencv_world480d.lib")
#else
#pragma comment(lib, "opencv_world480.lib")
#endif


class VideoClient {
private:
    std::unique_ptr<VideoService::Stub> stub_;// Client stub used to call the service

public:
    //cv::VideoCapture capture;

    VideoClient(const std::string& server_address  ) {
        std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
        stub_ = VideoService::NewStub(channel);

   /*     capture.open(video_path);
        if (!capture.isOpened()) {
            std::cout << "Error: Could not open the video." << std::endl;
        }*/
    }

    bool sendMat(const cv::Mat& mat) {
        Frame frame;
        std::vector<uchar> buffer;
        cv::imencode(".jpg", mat, buffer);
        frame.set_data(buffer.data(), buffer.size());

        Empty response;
        grpc::ClientContext context;
        grpc::Status status = stub_->SendFrame(&context, frame, &response);
        if (!status.ok()) {
            std::cerr << "Failed to send frame: " << status.error_message() << std::endl;
            return false;
        }
        return true;
    }
};

 

    int main() {
        // Load a sample image from OpenCV
        cv::Mat sampleMat = cv::imread("C:/Users/Moshe Sayada/Documents/repos/VideoFrameTransferProject/sample.jpg");
        cv::imshow("sample", sampleMat);
        cv::waitKey(0);

        if (sampleMat.empty()) {
            std::cerr << "Error: Couldn't open the image." << std::endl;
            return -1;
        }

        VideoClient client("localhost:50051"); // Assuming server is running on localhost at port 50051

       client.sendMat(sampleMat); // Send the image to the server

        std::cout << "Image sent." << std::endl;

        return 0;
    }

 

//int main() {
//     std::string video_path = "C:/Users/Moshe Sayada/Documents/repos/VideoFrameTransferProject/BLK-HDPTZ12 Security Camera Parkng Lot Surveillance Video.mp4";
//
//    std::cout << video_path << std::endl;
//
//    VideoClient client("localhost:50051", video_path);// Client setup and operations
//
//    cv::Mat frame;
//    while (true) {
//        client.capture >> frame; // Read a frame from the video
//
//        if (frame.empty()) {
//            break; // If the frame is empty, we've reached the end of the video
//        }
//
//        client.sendMat(frame); // Send the frame to the server
//        std::cout << "Frame sent." << std::endl;
//     }
//
//    std::cout << "Finished sending all frames." << std::endl;
//    return 0;
//}

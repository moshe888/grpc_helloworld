#include <grpcpp/grpcpp.h>
#include "video_service.grpc.pb.h"
#include <opencv2/opencv.hpp>
#include <queue>
#include <iostream>
#include <thread>
#include "JQueue.h"

#include <mutex>


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using video::VideoService;
using video::Empty;
using video::Frame;
 


class VideoServiceImpl final : public VideoService::Service {
private:
    JQueue frameQueue_;
    bool stopProcessing_ = false;  // A flag to stop the processing thread when shutting down

public:
    grpc::Status SendFrame(grpc::ServerContext* context, const Frame* frame, Empty* response) override {
        std::vector<uchar> buffer(frame->data().begin(), frame->data().end());
        cv::Mat img = cv::imdecode(buffer, cv::IMREAD_COLOR); 
        frameQueue_.push(img);
        return grpc::Status::OK;
    }

    void processingThread() {
        while (true) {
            if (stopProcessing_) return;  // End the thread if signaled to stop

            cv::Mat frame = frameQueue_.pop();
            // Process the frame here
        }
    }

    // Call this when shutting down to stop the processing thread
    void stopProcessing() {
        stopProcessing_ = true;
    }
};

class VideoServer {
private:
    std::unique_ptr<grpc::Server> server_;
    std::string server_address_;
    VideoServiceImpl service;
    std::thread processingThread_;  // The separate processing thread

public:
    VideoServer(const std::string& server_address) : server_address_(server_address) {}

    void run() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address_, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address_ << std::endl;

        // Start the frame processing thread
        processingThread_ = std::thread(&VideoServiceImpl::processingThread, &service);

        server_->Wait();

        // Stop the processing thread before shutting down
        service.stopProcessing();
        processingThread_.join();
    }

    void stop() {
        if (server_) {
            server_->Shutdown();
        }
    }
};

int main() {
    // Server setup and operations
    VideoServer server("0.0.0.0:50051");
    server.run();
    return 0;
}


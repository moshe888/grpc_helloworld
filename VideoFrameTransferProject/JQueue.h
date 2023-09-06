 
#pragma once

#include <queue>
#include <mutex>
//#include <condition_variable>
#include <opencv2/opencv.hpp>

class JQueue {
public:
    void push(const cv::Mat& frame);
    cv::Mat pop();
    bool empty() const;

private:
    std::queue<cv::Mat> imageQueue;
    std::mutex mtx;
    std::condition_variable cv;
};

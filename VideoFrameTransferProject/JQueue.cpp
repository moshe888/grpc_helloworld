 
#include "JQueue.h"
using namespace std;

void JQueue::push(const cv::Mat& frame) {
    std::unique_lock<std::mutex> lock(mtx);
     if (imageQueue.size() == 5) {
		imageQueue.pop();
	}
    imageQueue.push(frame);
    cv.notify_one(); 
}

cv::Mat JQueue::pop() {
    std::unique_lock<std::mutex> lock(mtx);
    while (imageQueue.empty()) {
        cout << "The queue is empty!\n";
        cv.wait(lock);//מכניס את התרד לשינה עד שהתור יהיה לא ריק
    }
    cv::Mat frame = imageQueue.front();
    cv::imshow("frame", frame);
    cv::waitKey(0);
    cout << "The queue is not empty!\n";
    imageQueue.pop();
    return frame;
}

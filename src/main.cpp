#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#define WIDTH 100     // Since characters are taller than they are wider its better if width ≈ height * 2
#define HEIGHT 40

const char *brightness_scale = " .,-~+=&%#$@$";

void clearScreen(void);

int main(int argc, char *argv[]) {
    int len = strlen(brightness_scale);
    std::string frame_buffer;
    frame_buffer.reserve(WIDTH * HEIGHT + HEIGHT);

    cv::VideoCapture cap("../asset/bad_apple.mp4");

    system("mpv --quiet ../asset/bad_apple.mp3 &");
    std::cout << "\033[?25l";  // Hide cursor
    while (true) {
        clearScreen();

        cv::Mat frame;
        bool ret = cap.read(frame);
        if (!ret) {
            std::cerr << "Error Opening Video\n";
            return -1;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::resize(frame, frame, cv::Size(WIDTH, HEIGHT), 0, 0, cv::INTER_AREA);

        frame_buffer.clear();
        for (int y = 0; y < frame.rows; y++) {
            for (int x = 0; x < frame.cols; x++) {
                uchar brightness = frame.at<uchar>(y, x);
                int index = static_cast<int>((brightness / 255.0) * (len - 1));
                frame_buffer += brightness_scale[index];
            }

            frame_buffer += '\n';
        }
        std::cout << frame_buffer;

        usleep(27500);
     }

     std::cout << "\033[?25h"; // Unhide cursor
     cap.release();

    return 0;
}

void clearScreen(void) {
    std::cout << "\033[H"; // move cursor top
    std::cout << "\033[0J"; // clear from cursor to end of screen
}
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstring>

#define WIDTH  160
#define HEIGHT 45

const char *brightness_scale = " .,-~+=&%#$@";

using clk = std::chrono::steady_clock;

static std::atomic<bool>  audio_ready{false};
static clk::time_point    start_time;

void moveCursorTop(void) {
    std::cout << "\033[H"; // move the cursor to the top
}

void startVideo(void) {
    while (!audio_ready.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int len = strlen(brightness_scale);
    std::string frame_buffer;
    frame_buffer.reserve(WIDTH * HEIGHT + HEIGHT);

    cv::VideoCapture cap("../asset/bad_apple.mp4");
    if (!cap.isOpened()) cap.open("asset/bad_apple.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video\n";
        return;
    }

    const double fps = cap.get(cv::CAP_PROP_FPS);

    while (true) {
        double elapsed      = std::chrono::duration<double>(clk::now() - start_time).count();
        int    target_frame = static_cast<int>(elapsed * fps);
        int    cur_frame    = static_cast<int>(cap.get(cv::CAP_PROP_POS_FRAMES));

        if (target_frame > cur_frame + 1) {
            cap.set(cv::CAP_PROP_POS_FRAMES, target_frame);
        }
        else if (target_frame < cur_frame) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        cv::Mat frame;
        if (!cap.read(frame)) break;

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::resize(frame, frame, cv::Size(WIDTH, HEIGHT), 0, 0, cv::INTER_AREA);

        frame_buffer.clear();
        for (int y = 0; y < frame.rows; y++) {
            for (int x = 0; x < frame.cols; x++) {
                uchar brightness = frame.at<uchar>(y, x);
                int index        = brightness * (len - 1) / 255;
                frame_buffer    += brightness_scale[index];
            }
            frame_buffer += '\n';
        }

        moveCursorTop();
        std::cout << frame_buffer << std::flush;

        double next_frame_t = (target_frame + 1) / fps;
        auto   wake_at      = start_time + std::chrono::duration_cast<clk::duration>(
                                  std::chrono::duration<double>(next_frame_t));
        std::this_thread::sleep_until(wake_at);
    }

    cap.release();
}

void startAudio(void) {
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << "\n";
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio: " << Mix_GetError() << "\n";
        SDL_Quit();
        return;
    }

    Mix_Music *music  = Mix_LoadMUS("../asset/bad_apple.mp3");
    if (!music) music = Mix_LoadMUS("asset/bad_apple.mp3");
    if (!music) {
        std::cerr << "Mix_LoadMUS: " << Mix_GetError() << "\n";
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }

    Mix_PlayMusic(music, 1);
    start_time = clk::now();
    audio_ready.store(true);

    while (Mix_PlayingMusic())
        SDL_Delay(100);

    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
}

int main(void) {
    std::cout << "\033[?25l"; // hide cursor

    std::thread video(startVideo);
    std::thread audio(startAudio);

    video.join();
    audio.join();

    std::cout << "\033[?25h"; // return cursor

    return 0;
}
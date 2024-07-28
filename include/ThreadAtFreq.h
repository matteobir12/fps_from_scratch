#ifndef THREAD_AT_FREQ_H
#define THREAD_AT_FREQ_H

#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <functional>

class ThreadAtFreq {
public:
    /**
     * @brief Construct a new Thread At Freq object
     * 
     * @param func Should not block
     * @param frequency 
     * @param threadID 
     */
    ThreadAtFreq(std::function<void()> func, int frequency, std::string threadID) : func_(func), frequency_(frequency), keepRunning_(false), threadID_(threadID) {}

    void start() {
        if (keepRunning_.load(std::memory_order_acquire)) {
            std::cout << threadID_ << " already running" << std::endl;
            return;
        }
        keepRunning_.store(true, std::memory_order_release);
        thread_ = std::thread(&ThreadAtFreq::run, this);
    }

    void stop() {
        if (!keepRunning_.load(std::memory_order_acquire)) {
            std::cout << threadID_ << " not running" << std::endl;
            return;
        }
        keepRunning_.store(false, std::memory_order_release);
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    ~ThreadAtFreq() {
        stop();
    }

private:
    void run() {
        using namespace std::chrono;

        int targetTime = 1000 / frequency_;
        /// steady_clock::time_point lastUpdateTime = steady_clock::now(); ???
        steady_clock::time_point lastWarningTime = steady_clock::now() - seconds(1);

        int lateCounter = 0;
        int loopCounter = 0;

        while (keepRunning_.load(std::memory_order_acquire)) {
            auto startTime = steady_clock::now();
            
            func_();

            auto endTime = steady_clock::now();
            auto elapsedTime = duration_cast<milliseconds>(endTime - startTime);
            auto sleepTime = targetTime - elapsedTime.count();

            if (sleepTime < 0) {
                lateCounter++;

                if(duration_cast<seconds>(steady_clock::now() - lastWarningTime).count() >= 1) {
                    std::cout << "Warning: "<< threadID_ <<" Running behind! ";
                    std::cout << "Loop was late " << lateCounter << " times out of " << loopCounter << " in the last second." << std::endl;
                    lastWarningTime = steady_clock::now();
                    lateCounter = 0;
                    loopCounter = 0;
                }
            } else {
                std::this_thread::sleep_for(milliseconds(sleepTime));
            }
        }
    }

    std::function<void()> func_;
    int frequency_;
    std::thread thread_;
    std::atomic<bool> keepRunning_;
    std::string threadID_;
};

#endif // THREAD_AT_FREQ_H
#include <iostream>
#include <queue>
#include <functional>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>

class TaskScheduler {
public:
    TaskScheduler() : stopFlag(false) {
        workerThread = std::thread(&TaskScheduler::run, this);
    }

    ~TaskScheduler() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopFlag = true;
        }
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    /// <summary>
    /// ��������� ������ � ����� � ������������ �������� ����������
    /// </summary>
    /// <param name="task">������ � ���� �������</param>
    /// <param name="timestamp">�����, �� ������ �������� ������ ������ ���� ���������</param>
    void Add(std::function<void()> task, std::time_t timestamp) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.emplace(timestamp, std::move(task));
        cv.notify_all();
    }

private:
    struct Task {
        std::time_t executeTime;
        std::function<void()> func;

        bool operator>(const Task& other) const {
            return executeTime > other.executeTime;
        }
    };

    std::priority_queue<Task, std::vector<Task>, std::greater<>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopFlag;
    std::thread workerThread;

    /// <summary>
    /// ��������� ����� �� ����������
    /// </summary>
    void run() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);

            // ���� �� ����������
            if (stopFlag && tasks.empty()) {
                break;
            }

            // �������� ������
            if (tasks.empty()) {
                cv.wait(lock);
            }
            else {
                auto now = std::time(nullptr);
                auto& nextTask = tasks.top();

                if (now >= nextTask.executeTime) {
                    auto task = nextTask.func;
                    tasks.pop();
                    lock.unlock();
                    task();
                }
                else {
                    cv.wait_until(lock, std::chrono::system_clock::from_time_t(nextTask.executeTime));
                }
            }
        }
    }
};
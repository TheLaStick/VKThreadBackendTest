#include <iostream>
#include <format>
#include "TaskScheduler.cpp"

int main() {
    TaskScheduler scheduler;

    scheduler.Add([]() { std::cout << "Task 1 executed\n"; }, std::time(nullptr) + 3); // Задача через 3 сек
    scheduler.Add([]() { std::cout << "Task 2 executed\n"; }, std::time(nullptr) + 5); // Задача через 5 сек
    scheduler.Add([]() { std::cout << "Task 3 executed\n"; }, std::time(nullptr) + 1); // Задача через 1 сек

    std::this_thread::sleep_for(std::chrono::seconds(5));

    scheduler.Add([]() { std::cout << "Task 4 executed\n"; }, std::time(nullptr) + 1); // Задача через 3 сек
    scheduler.Add([]() { std::cout << "Task 5 executed\n"; }, std::time(nullptr) + 5); // Задача через 5 сек
    scheduler.Add([]() { std::cout << "Task 6 executed\n"; }, std::time(nullptr) + 3); // Задача через 1 сек

    int seconds;

    std::this_thread::sleep_for(std::chrono::seconds(6));
    std::cout << "Enter amount of seconds for task to complete" << std::endl << "Enter -1 to exit" << std::endl;

    while (true) {
        std::cin >> seconds;
        if (seconds < 0) {
            std::cout << "Shutting down";
            return 0;
        }

        std::cout << std::format("User task started. Wait {} seconds\n", seconds);
        scheduler.Add([]() { std::cout << "User task complete." << std::endl; }, std::time(nullptr) + seconds);
    }

    return 0;
}

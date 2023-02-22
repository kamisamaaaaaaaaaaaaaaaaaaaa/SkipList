#include <iostream>
#include <chrono>
#include <cstdlib>
#include <Windows.h>
#include <time.h>
#include "database.h"
#include <mutex>

std::mutex mut;

#define THREAD_NUM 2
#define TEST_COUNT 1000000

int cnt = 0;

class Test :public DataBase<int, std::string> {
public:
    Test(int n) :DataBase<int, std::string>(n) {};

protected:
    virtual std::string encoding_key(int key) {
        return std::to_string(key);
    }
    
    virtual std::string encoding_value(std::string v) {
        return v;
    }

    virtual int decoding_key(std::string& s) {
        return stoi(s);
    }

    virtual std::string decoding_value(std::string& s) {
        return s;
    }

    virtual std::string key_items_showed(int k) {
        return std::to_string(k);
    }

    virtual std::string value_items_showed(std::string v) {
        return v;
    }
};

Test database(18);

DWORD WINAPI ThreadFunc1(LPVOID p) {
    int tmp = TEST_COUNT / THREAD_NUM;

    for (int count = 0; count < tmp; count++) {
        database.insert(rand() % TEST_COUNT, "a");
    }

    return 0;
}

DWORD WINAPI ThreadFunc2(LPVOID p) {
    int tmp = TEST_COUNT / THREAD_NUM;

    for (int count = 0; count < tmp; count++) {
        if (database.search(rand() % TEST_COUNT)) {
            mut.lock();
            cnt++;
            mut.unlock();
        };
    }

    return 0;
}

int main() {
    srand(time(NULL));
    {

        HANDLE hThread[THREAD_NUM];

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < THREAD_NUM; i++) {
            hThread[i] = CreateThread(NULL, 0, ThreadFunc1, NULL, 0, NULL);
        }

        WaitForMultipleObjects(THREAD_NUM, hThread, true, INFINITE);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
        //database.show_table();
    }

    srand(time(NULL));
    {

        HANDLE hThread[THREAD_NUM];

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < THREAD_NUM; i++) {
            hThread[i] = CreateThread(NULL, 0, ThreadFunc2, NULL, 0, NULL);
        }

        WaitForMultipleObjects(THREAD_NUM, hThread, true, INFINITE);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "search elapsed:" << elapsed.count() << "   found_elements:" << cnt << std::endl;
        //database.show_table();
    }

    return 0;

}
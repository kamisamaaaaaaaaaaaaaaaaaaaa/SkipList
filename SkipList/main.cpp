#include <iostream>
#include <string>
#include <Windows.h>
#include "database.h"
#define FILE_PATH "./store/dumpFile"

const int THREAD_NUM = 4;

class Test :public DataBase<int, std::string> {
public:
    Test(int n) :DataBase(n) {
    
    }
protected:
    virtual int parse_key(std::string& s) {
        return stoi(s);
    }

    virtual std::string parse_value(std::string& s) {
        return s;
    }

    virtual std::string key_items_showed(int k) {
        return std::to_string(k);
    }

    virtual std::string value_items_showed(std::string v) {
        return v;
    }
};

Test database(6);

template<typename K,typename V>
struct data {
    K k;
    V v;
};

DWORD WINAPI ThreadFunc(LPVOID p) {
    data<int, std::string>* pmd = (data<int, std::string>*)p;
    int key = pmd->k;
    std::string val = pmd->v;

    database.insert(key, val);

    return 0;
}

int main() {

    data<int,std::string> mydt[THREAD_NUM];
    HANDLE hThread[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; i++) {
        mydt[i].k = i;
        mydt[i].v = std::string(4, char('0' + i));
        hThread[i] = CreateThread(NULL, 0, ThreadFunc, &mydt[i], 0, NULL);
    }

    WaitForMultipleObjects(THREAD_NUM, hThread, true, INFINITE);

    //database.insert(1, "1111");
    //database.insert(3, "3333");
    //database.insert(4, "4444");
    //database.insert(5, "5555");

    std::cout << "database size:" << database.size() << std::endl;

    database.dump_file();

    database.load_file();

    database.search(9);
    database.search(3);


    database.show_table();

    database.remove(3);
    database.remove(7);

    std::cout << "database size:" << database.size() << std::endl;

    database.show_table();

    return 0;
}
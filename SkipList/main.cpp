#include <iostream>
#include <string>
#include "database.h"
#define FILE_PATH "./store/dumpFile"

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

int main() {

    Test database(6);
    database.insert(1, "1111");
    database.insert(3, "3333");
    database.insert(4, "4444");
    database.insert(5, "5555");

    std::cout << "database size:" << database.size() << std::endl;

    database.dump_file();

    database.load_file();

    database.search(9);
    database.search(18);


    database.show_table();

    database.remove(3);
    database.remove(7);

    std::cout << "database size:" << database.size() << std::endl;

    database.show_table();

    return 0;
}
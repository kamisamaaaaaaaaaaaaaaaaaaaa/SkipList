#include <iostream>
#include <string>
#include "skiplist.h"
#define FILE_PATH "./store/dumpFile"

class Test :public SkipList<int, std::string> {
public:
    Test(int n) :SkipList(n) {
    
    }
protected:
    virtual int parse_key(std::string& s) {
        return stoi(s);
    }

    virtual std::string parse_value(std::string& s) {
        return s;
    }

    virtual std::string node_display(Node<int,std::string>* node) {
        return "["+std::to_string(node->get_key())+"->"+node->get_value()+"]";
    }
};

int main() {

    Test skipList(6);
    skipList.insert_element(1, "1111");
    skipList.insert_element(3, "3333");
    skipList.insert_element(4, "4444");
    skipList.insert_element(5, "5555");

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.display_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();
}
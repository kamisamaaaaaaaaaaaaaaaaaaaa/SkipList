#pragma once
#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <string>

std::mutex mtx; 

template<typename K,typename V>
class Node {

public:

	Node() {};

	Node(K k, V v, int);

	~Node();

	K get_key() const;

	V get_value() const;

	void set_value(V);

    //指针数组forward[i]表示第i层该节点指向哪个节点
	Node<K, V>** forward;

	int node_level;

private:
	K key;
	V value;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level):key(k), value(v), node_level(level){
	forward = new Node<K, V>*[level + 1];
	memset(forward, 0, sizeof(Node<K, V>*) * (level + 1));
};

template<typename K, typename V>
Node<K, V>::~Node() {
	delete[]forward;
};

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
};

template<typename K, typename V>
V Node<K, V>::get_value() const {
	return value;
};

template<typename K, typename V>
void Node<K, V>::set_value(V value) {
	value = value;
};

template <typename K, typename V>
class SkipList {

public:
    template<typename CMP>
    SkipList(int n,CMP cmp);

    SkipList(int n);

    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    int size();
    Node<K, V>* get_header();


private:
    int _max_level;

    int _skip_list_level;

    Node<K, V>* _header;

    int _element_count;

    bool (*_cmp)(K, K);

    bool compare(K, K);

    bool equal(K, K);

    bool display;

protected:
   
    virtual std::string node_display(Node<K, V>* node) {
        return "";
    }

};

//小于return true
template<typename K, typename V>
bool SkipList<K, V>::compare(K k1,K k2) {
    try {
        if (_cmp) return _cmp(k1, k2);
        else return k1 < k2;
    }
    catch (...) {
        std::cout << "请传入正确的比较函数或重载<操作符" << std::endl;
    }
}

//=等价于!< && !>
template<typename K, typename V>
bool SkipList<K, V>::equal(K k1, K k2) {
    return !compare(k1, k2) && !compare(k2, k1);
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::get_header() {
    return _header;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V>* n = new Node<K, V>(k, v, level);
    return n;
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {

    mtx.lock();
    Node<K, V>* current = this->_header;

    //存取每层要插入位置的前一个点
    Node<K, V>** update = new Node<K, V>*[_max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

    //找到每一层的前一个点（<key的最大值）
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && compare(current->forward[i]->get_key(),key)) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    //走到底层往右走一格
    current = current->forward[0];

    //相等说明节点已存在，返回
    if (current != NULL && equal(current->get_key(),key)) {
        if(display) std::cout << "node exists:" << node_display(current) << std::endl;
        mtx.unlock();
        return 1;
    }

    //否则插入
    if (current == NULL || !equal(current->get_key(), key)) {

        //获取最高插入哪一层
        int random_level = get_random_level();

        //如果比已有层数大，先补几层，每层插入位置的前一个位置为header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i < random_level + 1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        Node<K, V>* inserted_node = create_node(key, value, random_level);

        //插入
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        if(display) std::cout << "Successfully inserted node:" << node_display(inserted_node) << std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

//显示底层链表
template<typename K, typename V>
void SkipList<K, V>::display_list() {
    if (!display) {
        std::cout << "please set display to true and define node_display function" << std::endl;
        return;
    }

    std::cout << "\n*****Skip List*****" << "\n";
    for (int i = 0; i <= _skip_list_level; i++) {
        Node<K, V>* node = _header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node_display(node) << "->";
            node = node->forward[i];
        }
        std::cout << "over" << std::endl;
    }
}

template<typename K, typename V>
int SkipList<K, V>::size() {
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {

    mtx.lock();
    Node<K, V>* current = _header;
    Node<K, V>** update = new Node<K,V>*[_max_level + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (_max_level + 1));

    //找到每层前一个位置
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && compare(current->forward[i]->get_key(),key)) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    //如果存在这个点
    if (current != NULL && equal(current->get_key(),key)) {

        //遍历每层，若有这个点则删掉，否则Break
        for (int i = 0; i <= _skip_list_level; i++) {

            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] = current->forward[i];
        }
        if(display) std::cout << "Successfully deleted node: " << node_display(current) << std::endl;
        delete current;

        //可能会把某一层的节点删完，此时层数减1
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level--;
        }
        _element_count--;
    }
    mtx.unlock();
    return;
}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {

    //std::cout << "search_element-----------------" << std::endl;
    Node<K, V>* current = _header;

    //找到每层对应节点的前一个位置
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && compare(current->forward[i]->get_key(), key)) {
            current = current->forward[i];
        }
    }

    //底层往右一格
    current = current->forward[0];

    if (current and equal(current->get_key(), key)) {
        if(display) std::cout << "Found key: " << node_display(current) << std::endl;
        return true;
    }

    std::cout << "Not Found" << std::endl;
    return false;
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level):_max_level(max_level), _skip_list_level(0), _element_count(0),_cmp(NULL),display(false){
    K k{};
    V v{};
    this->_header = new Node<K, V>(k, v, _max_level);
};

template<typename K, typename V>
template<typename CMP>
SkipList<K, V>::SkipList(int max_level, CMP cmp):_max_level(max_level), _skip_list_level(0), _element_count(0),_cmp(cmp), display(false){
    K k{};
    V v{};
    this->_header = new Node<K, V>(k, v, _max_level);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    delete _header;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {

    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};
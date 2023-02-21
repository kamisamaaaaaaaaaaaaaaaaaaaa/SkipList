#pragma once

#include "skiplist.h"
#include <iostream>
#include <pthread.h>

#define MAX_THREADS 500
#define STORE_FILE "store/dumpFile"

std::string delimiter = ":";

template<typename K,typename V>
class DataBase {
public:
	DataBase(int n);
	DataBase(int n,bool flag);

	~DataBase();

	void dump_file();
	void load_file();
	int insert(K k,V v);
	void remove(K k);
	bool search(K k);
	void show_table();
	int size();
	void set_num_threads(int);

	bool multi_thread;
	int num_threads;

private:
	std::ofstream _file_writer;
	std::ifstream _file_reader;

	std::unique_ptr<SkipList<K, V>> skiplist;

	void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
	bool is_valid_string(const std::string& str);


protected:
	virtual K parse_key(std::string& s) {
		K key{};
		return key;
	}

	virtual V parse_value(std::string& s) {
		V value{};
		return value;
	}

	virtual std::string key_items_showed(K k) {
		return "";
	}

	virtual std::string value_items_showed(V v) {
		return "";
	}
};

template<typename K,typename V>
DataBase<K, V>::DataBase(int n) :skiplist(std::make_unique<SkipList<K, V>>(n)), multi_thread(false) {};

template<typename K, typename V>
DataBase<K, V>::DataBase(int n,bool flag) :skiplist(std::make_unique<SkipList<K, V>>(n)), multi_thread(flag) {
	if (flag) num_threads = 100;
};

template<typename K, typename V>
void DataBase<K, V>::set_num_threads(int num) {
	num_threads = min(num,MAX_THREADS);
}

template<typename K,typename V>
DataBase<K, V>::~DataBase() {
	if (_file_writer.is_open()) {
		_file_writer.close();
	}
	if (_file_reader.is_open()) {
		_file_reader.close();
	}

	skiplist.reset();
};

template<typename K, typename V>
int DataBase<K, V>::insert(K k, V v) {
	return skiplist->insert_element(k, v);
}

template<typename K, typename V>
void DataBase<K, V>::remove(K k) {
	skiplist->delete_element(k);
}

template<typename K, typename V>
bool DataBase<K, V>::search(K k) {
	return skiplist->search_element(k);
}

template<typename K, typename V>
void DataBase<K, V>::dump_file() {
	std::cout << "dump_file-----------------" << std::endl;
	_file_writer.open(STORE_FILE);
	Node<K, V>* node = skiplist->get_header()->forward[0];

	while (node != NULL) {
		_file_writer << node->get_key() << ":" << node->get_value() << "\n";
		std::cout << node->get_key() << ":" << node->get_value() << ";\n";
		node = node->forward[0];
	}

	_file_writer.flush();
	_file_writer.close();
	return;
};

template<typename K, typename V>
void DataBase<K, V>::load_file() {
	_file_reader.open(STORE_FILE);
	std::cout << "load_file-----------------" << std::endl;
	std::string line;
	std::string* key = new std::string();
	std::string* value = new std::string();

	while (getline(_file_reader, line)) {
		get_key_value_from_string(line, key, value);
		if (key->empty() || value->empty()) {
			continue;
		}
		insert(parse_key(*key), parse_value(*value));
		std::cout << "key:" << *key << " " << "value:" << *value << std::endl;
	}
	_file_reader.close();
};

template<typename K, typename V>
void DataBase<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

	if (!is_valid_string(str)) {
		return;
	}
	*key = str.substr(0, str.find(delimiter));
	*value = str.substr(str.find(delimiter) + 1, str.length());
}

template<typename K, typename V>
bool DataBase<K, V>::is_valid_string(const std::string& str) {

	if (str.empty()) {
		return false;
	}
	if (str.find(delimiter) == std::string::npos) {
		return false;
	}
	return true;
}


template<typename K, typename V>
void DataBase<K, V>::show_table() {
	printf("*****Table*****\n");
	printf("----------------------------------\n");
	printf("  key          |           value\n");

	Node<K,V> *node=skiplist->get_header()->forward[0];
	while (node != NULL) {
		printf("----------------------------------\n");
		std::cout << "   " << key_items_showed(node->get_key());
		printf("          |            ");
		std::cout << value_items_showed(node->get_value());
		printf("\n");
		node = node->forward[0];
	}
	printf("----------------------------------\n");
}

template<typename K, typename V>
int DataBase<K, V>::size() {
	return skiplist->size();
}

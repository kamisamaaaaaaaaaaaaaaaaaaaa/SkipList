#pragma once

#include "skiplist.h"
#include <iostream>
#include <Windows.h>
#define STORE_FILE "store/dumpFile"

std::string delimiter = ":";

template<typename K,typename V>
class DataBase {
public:
	template<typename CMP>
	DataBase(int n,CMP cmp);

	DataBase(int n);

	~DataBase();

	void dump_file();
	void load_file();
	int insert(K k,V v);
	void remove(K k);
	bool search(K k);
	void show_table();
	int size();

private:
	std::ofstream _file_writer;
	std::ifstream _file_reader;

	SkipList<K, V>* skiplist;

	void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
	bool is_valid_string(const std::string& str);


protected:
	virtual std::string encoding_key(K key) {
		return "";
	}

	virtual std::string encoding_value(V v) {
		return "";
	}

	virtual K decoding_key(std::string& s) {
		K key{};
		return key;
	}

	virtual V decoding_value(std::string& s) {
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
template<typename CMP>
DataBase<K, V>::DataBase(int n,CMP cmp){
	skiplist = new SkipList<K,V>(n,cmp);
};

template<typename K, typename V>
DataBase<K, V>::DataBase(int n) {
	skiplist = new SkipList<K, V>(n);
};

template<typename K,typename V>
DataBase<K, V>::~DataBase() {
	if (_file_writer.is_open()) {
		_file_writer.close();
	}
	if (_file_reader.is_open()) {
		_file_reader.close();
	}

	delete skiplist;
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
		_file_writer << encoding_key(node->get_key()) << ":" << encoding_value(node->get_value()) << "\n";
		std::cout << encoding_key(node->get_key()) << ":" << encoding_value(node->get_value()) << ";\n";
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
		insert(decoding_key(*key), decoding_value(*value));
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

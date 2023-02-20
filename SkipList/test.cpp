#include <iostream>
#include <cstring>
#include <string>

using namespace std;

class self_defined1 {
public:
	int a;
	int b;

	self_defined1() :a(1), b(2) {};

};

class self_defined2 {
public:
	string a;
	string b;

	self_defined2() :a(""), b("") {};

};


template<typename K,typename V>
class Test {
public:
	K key;
	V value;

	virtual K deserialize_key(string& s) {
		K key{};
		return key;
	}

	virtual V deserialize_value(string& s) {
		V value{};
		return value;
	}
};

class sub_class :public Test<self_defined1, self_defined2> {
public:
	virtual self_defined1 deserialize_key(string s) {
		self_defined1 res;
		res.a = stoi(s), res.b = stoi(s);
		return res;
	}

	virtual self_defined2 deserialize_value(string s) {
		self_defined2 res;
		res.a = s, res.b = s;
		return res;
	}
};

int main() {
	sub_class test;

	auto a=test.deserialize_key("123");
	auto b = test.deserialize_value("568");

	cout << a.a << " " << a.b << endl;
	cout << b.a << " " << b.b << endl;

}
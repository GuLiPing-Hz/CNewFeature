// CNewFeature.cpp : �������̨Ӧ�ó������ڵ㡣
//


/************************************************************************/
/* 
	1. �ɱ䳤ģ���������
	2. std::function,std::bind ���� 
	3. std::move ����
	4. ��ֵ����

*/
/************************************************************************/

#include "stdafx.h"
#include <string>
#include <stdio.h>
#include <functional>
#include <vector>
#include <iostream>
#include <random>
#include <memory>

class C11{

public:
	C11(){

	}

	//�ɱ䳤ģ���������
	template<typename... Tx>
	static void test1(const char* format, Tx... param){
		printf(format, param...);

		//�ݹ����getJNISignature��������������
		printf("signature = %s\n", getJNISignature(param...).c_str());
	}

	static std::string getJNISignature() {
		return "";
	}

	static std::string getJNISignature(bool) {
		return "Z";
	}

	static std::string getJNISignature(char) {
		return "C";
	}

	static std::string getJNISignature(short) {
		return "S";
	}

	static std::string getJNISignature(int) {
		return "I";
	}

	static std::string getJNISignature(long) {
		return "J";
	}

	static std::string getJNISignature(float) {
		return "F";
	}

	static  std::string getJNISignature(double) {
		return "D";
	}

	static std::string getJNISignature(const char*) {
		return "Ljava/lang/String;";
	}

	static std::string getJNISignature(const std::string&) {
		return "Ljava/lang/String;";
	}

	template <typename T>
	static std::string getJNISignature(T x) {
		// This template should never be instantiated
		static_assert(sizeof(x) == 0, "Unsupported argument type");
		return "";
	}

	template <typename T, typename... Ts>
	static std::string getJNISignature(T x, Ts... param) {
		return getJNISignature(x) + getJNISignature(param...);
	}

	//std::function
	std::function<void()> mFunc;
	void setFunc(std::function<void()> lam){
		mFunc = lam;
	}

	void test2(){
		if (mFunc)
			mFunc();
	}

};

void f(int n1, int n2, int n3, const int& n4, int n5)
{
	std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int n1)
{
	return n1 + 1;
}

struct Foo {
	void print_sum(int n1, int n2)
	{
		std::cout << n1 << "+" << n2 << "=" << n1 + n2 << '\n';
	}
	int data = 10;
};

struct Value {
	void lrValue(int &a){//��ֵ����
		std::cout << "left value " << a << std::endl;
	}
	void lrValue(int &&a){//��ֵ����
		std::cout << "right value " << a << std::endl;
		std::cout << "right value " << ++a << std::endl;//�ɱ�

		lrValue(++a);//�������ã�֤��a�������Ѿ�����ֵ��
	}
};

void TestBind(){
	printf("******************���� std::bind****************************\n");
	//std::bind����
	int n = 7;
	// (_1 and _2 are from std::placeholders, and represent future
	// arguments that will be passed to f1)
	auto f1 = std::bind(f, std::placeholders::_2, std::placeholders::_1, 42, std::cref(n), n);
	n = 10;
	f1(1, 2, 1001);

	auto f2 = std::bind(f, std::placeholders::_3, std::bind(g, std::placeholders::_3), std::placeholders::_3, 4, 5);
	f2(10, 11, 12);

	// common use case: binding a RNG with a distribution
	std::default_random_engine e;//�����
	std::uniform_int_distribution<> d(0, 10);
	std::function<int()> rnd = std::bind(d, e); // a copy of e is stored in rnd
	for (int n = 0; n < 10; ++n)
		std::cout << rnd() << ' ';
	std::cout << '\n';

	// bind to a pointer to member function
	Foo foo;
	auto f3 = std::bind(&Foo::print_sum, &foo, 95, std::placeholders::_1);
	f3(5);
}

void TestTemplate(){

	printf("*********************�ɱ䳤ģ�����*************************\n");
	//�ɱ䳤ģ�����
	C11::test1("[%s] %d + %d = %d\n", "add", 1, 2, 3);
	const char* s1 = "123456";

	auto lam = [s1]()->void{
		printf("%s\n", s1);
	};
	C11 obj;
	obj.setFunc(lam);
	obj.test2();
}

void TestMove(){
	printf("********************���� std::move**************************\n");
	std::string str = "Hello";
	std::vector<std::string> v;

	// uses the push_back(const T&) overload, which means 
	// we'll incur the cost of copying str
	v.push_back(str);
	std::cout << "��ǰ�����, str is \"" << str << "\"\n";

	// uses the rvalue reference push_back(T&&) overload, 
	// which means no strings will be copied; instead, the contents
	// of str will be moved into the vector.  This is less
	// expensive, but also means str might now be empty.
	v.push_back(std::move(str));
	std::cout << "����c++11��move, str is \"" << str << "\"\n";

	std::cout << "���ǵ��ַ������� \"" << v[0] << "\", \"" << v[1] << "\"\n";
}

void TestRightValue(){
	printf("********************���� ��ֵ����**************************\n");
	Value va;
	int val = 10;
	va.lrValue(val);//��ֵ����
	/*
	���û�ж��� lrValue(int && n)//���ǽ�����ʹ�������д��
	*/
	va.lrValue(10);//��ֵ����
}

void TestXor(){
	printf("********************���� ���**************************\n");
	char p1[] = "123abc";
	const char* key = "xxxxx";
	std::cout << "ԭ�����ַ���:" << p1 << std::endl;

	int keyLen = strlen(key);
	for (int i=0; i < strlen(p1); i++){
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "���1�ε��ַ���:" << p1 << std::endl;

	for (int i=0; i < strlen(p1); i++){
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "���2�ε��ַ���:" << p1 << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestXor();
	TestBind();
	TestMove();
	TestRightValue();
	TestTemplate();

	getchar();

	return 0;
}


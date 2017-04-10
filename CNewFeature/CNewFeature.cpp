// CNewFeature.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <stdio.h>
#include <functional>

class C11{

public:
	C11(){

	}

	//可变长模板参数测试
	template<typename... Tx>
	static void test1(const char* format, Tx... param){
		printf(format, param...);

		//递归调用getJNISignature，参数从右往左
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


	std::function<void()> mFunc;
	void setFunc(std::function<void()> lam){
		mFunc = lam;
	}

	void test2(){
		if (mFunc)
			mFunc();
	}

};

int _tmain(int argc, _TCHAR* argv[])
{

	C11::test1("[%s] %d + %d = %d\n","add",1,2,3);

	const char* s1 = "123456";

	auto lam = [s1]()->void{
		printf("%s\n", s1);
	};

	C11 obj;
	obj.setFunc(lam);
	obj.test2();

	getchar();

	return 0;
}


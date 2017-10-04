// CNewFeature.cpp : 定义控制台应用程序的入口点。
//


/************************************************************************/
/* 
	1. 可变长模板参数测试
	2. std::function,std::bind 测试 
	3. std::move 测试
	4. 右值引用

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
#include <thread>
#include <windows.h>
#include <map>

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
	void lrValue(int &a){//左值引用
		std::cout << "left value " << a << std::endl;
	}
	void lrValue(int &&a){//右值引用
		std::cout << "right value " << a << std::endl;
		std::cout << "right value " << ++a << std::endl;//可变

		lrValue(++a);//继续调用，证明a在这里已经是左值了
	}
};

void TestBind(){
	printf("******************测试 std::bind****************************\n");
	//std::bind测试
	int n = 7;
	// (_1 and _2 are from std::placeholders, and represent future
	// arguments that will be passed to f1)
	auto f1 = std::bind(f, std::placeholders::_2, std::placeholders::_1, 42, std::cref(n), n);
	n = 10;
	f1(1, 2, 1001);

	auto f2 = std::bind(f, std::placeholders::_3, std::bind(g, std::placeholders::_3), std::placeholders::_3, 4, 5);
	f2(10, 11, 12);

	// common use case: binding a RNG with a distribution
	std::default_random_engine e;//随机数
	std::uniform_int_distribution<> d(0, 10);
	std::function<int()> rnd = std::bind(d, e); // a copy of e is stored in rnd
	for (int n = 0; n < 10; ++n)
		std::cout << rnd() << ' ';
	std::cout << '\n';

	// bind to a pointer to member function
	Foo foo;
	auto f3 = std::bind(&Foo::print_sum, &foo, 95, std::placeholders::_1);
	f3(5);

	std::function<int()> f4 = std::bind(g, 1);
	printf("f4 = %d\n", f4());
}

void TestTemplate(){

	printf("*********************可变长模板参数*************************\n");
	//可变长模板参数
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
	printf("********************测试 std::move**************************\n");
	std::string str = "Hello";
	std::vector<std::string> v;

	// uses the push_back(const T&) overload, which means 
	// we'll incur the cost of copying str
	v.push_back(str);
	std::cout << "以前的深拷贝, str is \"" << str << "\"\n";

	// uses the rvalue reference push_back(T&&) overload, 
	// which means no strings will be copied; instead, the contents
	// of str will be moved into the vector.  This is less
	// expensive, but also means str might now be empty.
	v.push_back(std::move(str));
	std::cout << "现在c++11的move, str is \"" << str << "\"\n";

	std::cout << "我们的字符串队列 \"" << v[0] << "\", \"" << v[1] << "\"\n";
}

void TestRightValue(){
	printf("********************测试 右值引用**************************\n");
	Value va;
	int val = 10;
	va.lrValue(val);//左值引用
	/*
	如果没有定义 lrValue(int && n)//我们将不能使用下面的写法
	*/
	va.lrValue(10);//右值引用
}

void TestXor(){
	printf("********************测试 异或**************************\n");
	char p1[] = "123abc";
	const char* key = "xxxxx";
	std::cout << "原来的字符串:" << p1 << std::endl;

	int keyLen = strlen(key);
	for (int i=0; i < strlen(p1); i++){
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "异或1次的字符串:" << p1 << std::endl;

	for (int i=0; i < strlen(p1); i++){
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "异或2次的字符串:" << p1 << std::endl;
}

void TestVector(){
	printf("********************测试 Vector**************************\n");
	std::vector<unsigned char> vect;
	std::string str;
	vect.assign(5, 'a');
	unsigned char* p = vect.data();
	str.insert(0, (const char*)p, vect.size());
	printf("p=%s,size=%d\n", str.c_str(), vect.size());

	const char* p1 = "bcdef";
	vect.insert(vect.end(), (const unsigned char*)p1, (const unsigned char*)p1+strlen(p1));
	p = vect.data();
	str.insert(str.begin(), vect.begin(),vect.end());
	printf("p=%s,size=%d\n", str.c_str(), vect.size());
}

struct BufferUnit{
	enum DataType
	{
		type_char = 1,//[16+1][数组长度][data]
		type_short = 2,
		type_int = 3,
		type_int64 = 4,
		type_float = 5,//[]
		type_custom = 6,//[16+6][数组长度][{结构长度,结构体}]...

		type_array = 16,//
	};

	BufferUnit(){ memset(&data, 0, sizeof(data)); }
	virtual ~BufferUnit(){}

	int type;

	union Data
	{
		char c;
		short s;
		int i;
		long long ll;
		float f;
	};

	Data data;
	//字符串需要另外存放
	std::string str;
};

void printUnion(BufferUnit& bu){
	printf("union %c(%d),%d,%d,%lld,%f\n", bu.data.c, bu.data.c, bu.data.s, bu.data.i
		, bu.data.ll, bu.data.f);
}

void TestUnion(){
	printf("********************测试 联合和map无效的key值************************** %s\n", __FUNCTION__);

	BufferUnit bu;
	printf("bu.size = %d,union size = %d,type size = %d\n", sizeof(bu), sizeof(bu.data), sizeof(BufferUnit::DataType));
	bu.data.c = 48;
	bu.data.s = 48;
	bu.data.i = 48;
	bu.data.ll = 48;
	printUnion(bu);

	std::map<int, int> m;
	m[1] = 1;
	m[2] = 2;
	int ret = m[3];//string 返回""
}

typedef unsigned int t_uint32;
typedef int t_int32;
typedef long long t_int64;
typedef unsigned char t_byte;

struct TUserData
{
	t_int32	     dwUserID;            //ID 号码		0 + 4
	t_int32	     dwExperience;        //经验值		4 + 4
	t_int32	     dwAccID;             //ACC 号码		8 + 4
	t_int32      dwPoint;             //分数 12 + 4
	t_int64	     i64Money;            //金币 16+8
	t_int64	     i64Bank;             //银行	 24+8
	t_int64      i64AllMoney;		  //银行金币总数（钱包+存款） 32+8
	t_int64      i64TotalMoney;       //总金币      40+8
	t_int64	     i64Ingot;            //用户元宝数量 48+8
	t_uint32	 uWinCount;           //胜利数目
	t_uint32	 uLostCount;          //输数目
	t_uint32	 uCutCount;           //强退数目
	t_uint32	 uMidCount;           //和局数目
	char         szName[61];          //登录名
	char         szClassName[61];     //游戏社团
	t_uint32     bLogoID;             //头像 ID 号码
	t_byte       bDeskNO;             //游戏桌号
	t_byte       bDeskStation;        //桌子位置
	t_byte       bUserState;          //用户状态
	t_byte       bMember;             //会员等级
	t_byte       bGameMaster;         //管理等级
	t_uint32     dwUserIP;            //登录IP地址
	bool 	     bBoy;                //性别
	char 	     nickName[61]; 	      //用户昵称
	t_uint32 	 uDeskBasePoint; 	  //设置的桌子倍数
	t_int32 	 dwFascination; 	  //魅力
	t_int32 	 iVipTime; 		      //会员时间
	t_int32 	 iDoublePointTime;    //双倍积分时间
	t_int32 	 iProtectTime; 	      //护身符时间，保留
	t_int32 	 isVirtual; 		  //是否是扩展机器人
	t_uint32 	 dwTax;				  //房费
	char         szOccupation[61];    //玩家职业
	char         szPhoneNum[61];      //玩家电话号码
	char         szProvince[61];      //玩家所在的省
	char         szCity[61];          //玩家所在的市
	char         szZone[61];          //玩家所在的地区
	bool         bHaveVideo;          //是否具有摄像头
	char         szSignDescr[128];    //个性签名
	t_int32      userType;            //玩家类型信息(0 ,普通玩家;1 ,电视比赛玩家;2 ,VIP玩家;3 ,电视比赛VIP玩家)
	t_uint32     userInfoEx1;         //扩展字段1，用于邮游钻石身份作用时间，由黄远松添加
	t_uint32     userInfoEx2;         //扩展字段2，用于GM处理之禁言时效，由zxd添加于20100805
	t_int32		 bTrader;		      //用于判断是不是银商
	bool         bIsCanTransBack;     //是否合伙人      
	t_int64	     i64ContestScore;     //比赛成绩
	t_int32	     iContestCount;       //比赛局数
	t_int64      timeLeft;            //比赛剩余时间
	t_int64      i64TimeEnd;          //比赛结束时间
	t_int64      i64TimeStart;        //比赛开始时间
	t_int32		 iRankNum;		      //排行名次
};

struct TLoginResult
{
	t_uint32     dwGamePower;             //用户权限 4
	t_uint32     dwMasterPower;           //管理权限 8

	t_uint32     dwRoomRule;              //设置规则 12
	t_uint32     uLessPoint;              //最少经验值 16

	t_uint32     uMaxPoint;               //最多经验值 20
	
	//对齐，留空4字节
	TUserData    UserInfoStruct;	      //用户信息   20+808=828

	bool         rm_bISOpen;    	      //自动赠送专用 (rm = Receive Money)
	
	//对齐，留空7字节
	t_int64      rm_i64Money;             //自动赠送专用,?

	t_int64      rm_i64MinMoney;          //自动赠送专用,?

	t_int32      rm_iCount;               //自动赠送专用,?
	t_int32      rm_iTotal;               //自动赠送专用,?

	t_int32      rm_iTime;                //自动赠送专用,?
	t_int32      rm_iResultCode;          //自动赠送专用,?

	//[-- 
	t_int32      rm_iLessPoint;           //自动赠送专用,?
	char         mbi_szMatchDesp[200];    //赛制详情	(mbi = Match Base Info)
	t_int32      mbi_nReportCounts;       //报名人数
	//--] 208字节 无需对齐补齐

	t_int32      mbi_nMatchActors;        //参赛人数	
	t_int32      mbi_nScoreDiff;          //距离上几名的积分差,默认上一名

	t_int32      mbi_nPeopleLimit;        //开赛人数限制	
	bool         mbi_ISBegin;             //人未满是否仍开赛 
	bool         mbi_ISMatchEnd;          //比赛是否已经结束
	
	//对齐 留空2字节
	t_int32      nVirtualUser;            //登录房间时即时获取虚拟玩家人数
	t_int32      nPresentCoinNum;         //赠送金币数量

	t_int32	     iContestID;              //比赛专用,比赛ID
	t_int32      iLowCount;               //比赛专用,?

	t_int64	     i64Chip;                 //比赛专用,?

	t_int64	     i64TimeStart;            //比赛开始时间

	t_int64	     i64TimeEnd;              //比赛结束时间

	t_int64	     i64LowChip;              //比赛淘汰分数线

	t_int32	     iTimeout;                //比赛进场时限
	t_int32	     iBasePoint;              //房间底分
};

#define OFFSET(struc,e) ((int)(&((struc*)0)->e))
void TestStruct(){
	printf("********************测试 Struct**************************\n");
	TUserData a1 = { 0 };
	TLoginResult a2 = { 0 };

	int t0 = OFFSET(TUserData, iRankNum);
	int t1 = OFFSET(TLoginResult, uMaxPoint);
	int t2 = OFFSET(TLoginResult, UserInfoStruct);
	int t3 = OFFSET(TLoginResult, rm_bISOpen);
	int t4 = OFFSET(TLoginResult, rm_i64Money);
	int t41 = OFFSET(TLoginResult, rm_iLessPoint);


	int t421 = OFFSET(TLoginResult, mbi_szMatchDesp);
	int t422 = OFFSET(TLoginResult, mbi_nReportCounts);
	int t42 = OFFSET(TLoginResult, mbi_ISMatchEnd);


	int t5 = OFFSET(TLoginResult, nVirtualUser);
	int t6 = OFFSET(TLoginResult, mbi_szMatchDesp);
	int tEnd = OFFSET(TLoginResult, iBasePoint);
	

	int s1 = sizeof(a1);
	int s2 = sizeof(a2);
}

int g_Cnt = 0;

#include <mutex>

struct ThreadT{
	std::mutex* mutex;
	int step;
};


void AddThread(ThreadT& t,int step){
	for (int i = 0; i < 10; i++)
	{
		std::lock_guard<std::mutex> guard(*t.mutex);
		printf("********************************************%x \n", std::this_thread::get_id());
		printf("AddThread1 ,%d, %d\n", g_Cnt,t.step);
		g_Cnt += step;
		printf("AddThread2 ,%d\n", g_Cnt);
		Sleep(500);
	}
	
}

void SubThread(void* v){
	ThreadT* t = (ThreadT*)v;
	for (int i = 0; i < 10; i++)
	{
		std::lock_guard<std::mutex> guard(*t->mutex);
		printf("******************************************** %x \n",std::this_thread::get_id());
		printf("SubThread1 ,%d\n", g_Cnt);
		g_Cnt -= 1;
		printf("SubThread2 ,%d\n", g_Cnt);
		Sleep(500);
		//std::this_thread::sleep_until()
	}
}

std::mutex g_mutex;
ThreadT g_t;

void TestThread(){
	printf("********************测试 多线程************************** %x\n",std::this_thread::get_id());

	g_t.mutex = &g_mutex;
	g_t.step = 4;

	std::thread add = std::thread(AddThread, std::ref(g_t), 2);
	std::thread sub = std::thread(SubThread, &g_t);

	//分离线程
	add.detach();
	sub.detach();

	//等待子线程结束
	//add.join();
	//sub.join();
}

#define	OP_LITTLEENDIAN	'<'		/* little endian */
#define	OP_BIGENDIAN	'>'		/* big endian */
#define	OP_NATIVE	'='		/* native endian */

int doendian(int c)
{
	int x = 1;
	int e = *(char*)&x;
	if (c == OP_LITTLEENDIAN) return !e;
	if (c == OP_BIGENDIAN) return e;
	if (c == OP_NATIVE) return 0;
	return 0;
}

void doswap(int swap, void *p, size_t n)
{
	if (swap)
	{
		char *a = (char*)p;
		int i, j;
		for (i = 0, j = n - 1, n = n / 2; n--; i++, j--)
		{
			char t = a[i]; a[i] = a[j]; a[j] = t;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	short num1 = 0x1234;
	int num2 = 0x12345678;
	long long num3 = 0x123456789abcdeff;
	
	char* p = (char*)&num1;
	printf("&num1 = %#x\n", p);
	p = (char*)&num2;
	printf("&num2 = %#x\n", p);
	p = (char*)&num3;
	printf("&num3 = %#x\n", p);
	unsigned char* pU = (unsigned char*)p;
	/*
		内存地址从	小 ---->  大

		数据高位放在高位地址 0x9f < 0x8f  产出 < 小端
		数据高位放在低位地址 0x8f > 0x9f  产出 > 大端
	*/
	unsigned char big0 = p[0];
	printf("小端 %#x : %x,%#X,%x,%x\n", big0, p[0] & 0xff, p[1] & 0xff, pU[2], p[3]);

	TestXor();
	TestBind();
	TestMove();
	TestRightValue();
	TestTemplate();
	TestVector();
	TestUnion();
	TestStruct();
	TestThread();

	getchar();

	return 0;
}


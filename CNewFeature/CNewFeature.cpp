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
#include <thread>
#include <windows.h>
#include <map>

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

	std::function<int()> f4 = std::bind(g, 1);
	printf("f4 = %d\n", f4());
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

void TestVector(){
	printf("********************���� Vector**************************\n");
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
		type_char = 1,//[16+1][���鳤��][data]
		type_short = 2,
		type_int = 3,
		type_int64 = 4,
		type_float = 5,//[]
		type_custom = 6,//[16+6][���鳤��][{�ṹ����,�ṹ��}]...

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
	//�ַ�����Ҫ������
	std::string str;
};

void printUnion(BufferUnit& bu){
	printf("union %c(%d),%d,%d,%lld,%f\n", bu.data.c, bu.data.c, bu.data.s, bu.data.i
		, bu.data.ll, bu.data.f);
}

void TestUnion(){
	printf("********************���� ���Ϻ�map��Ч��keyֵ************************** %s\n", __FUNCTION__);

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
	int ret = m[3];//string ����""
}

typedef unsigned int t_uint32;
typedef int t_int32;
typedef long long t_int64;
typedef unsigned char t_byte;

struct TUserData
{
	t_int32	     dwUserID;            //ID ����		0 + 4
	t_int32	     dwExperience;        //����ֵ		4 + 4
	t_int32	     dwAccID;             //ACC ����		8 + 4
	t_int32      dwPoint;             //���� 12 + 4
	t_int64	     i64Money;            //��� 16+8
	t_int64	     i64Bank;             //����	 24+8
	t_int64      i64AllMoney;		  //���н��������Ǯ��+�� 32+8
	t_int64      i64TotalMoney;       //�ܽ��      40+8
	t_int64	     i64Ingot;            //�û�Ԫ������ 48+8
	t_uint32	 uWinCount;           //ʤ����Ŀ
	t_uint32	 uLostCount;          //����Ŀ
	t_uint32	 uCutCount;           //ǿ����Ŀ
	t_uint32	 uMidCount;           //�;���Ŀ
	char         szName[61];          //��¼��
	char         szClassName[61];     //��Ϸ����
	t_uint32     bLogoID;             //ͷ�� ID ����
	t_byte       bDeskNO;             //��Ϸ����
	t_byte       bDeskStation;        //����λ��
	t_byte       bUserState;          //�û�״̬
	t_byte       bMember;             //��Ա�ȼ�
	t_byte       bGameMaster;         //����ȼ�
	t_uint32     dwUserIP;            //��¼IP��ַ
	bool 	     bBoy;                //�Ա�
	char 	     nickName[61]; 	      //�û��ǳ�
	t_uint32 	 uDeskBasePoint; 	  //���õ����ӱ���
	t_int32 	 dwFascination; 	  //����
	t_int32 	 iVipTime; 		      //��Աʱ��
	t_int32 	 iDoublePointTime;    //˫������ʱ��
	t_int32 	 iProtectTime; 	      //�����ʱ�䣬����
	t_int32 	 isVirtual; 		  //�Ƿ�����չ������
	t_uint32 	 dwTax;				  //����
	char         szOccupation[61];    //���ְҵ
	char         szPhoneNum[61];      //��ҵ绰����
	char         szProvince[61];      //������ڵ�ʡ
	char         szCity[61];          //������ڵ���
	char         szZone[61];          //������ڵĵ���
	bool         bHaveVideo;          //�Ƿ��������ͷ
	char         szSignDescr[128];    //����ǩ��
	t_int32      userType;            //���������Ϣ(0 ,��ͨ���;1 ,���ӱ������;2 ,VIP���;3 ,���ӱ���VIP���)
	t_uint32     userInfoEx1;         //��չ�ֶ�1������������ʯ�������ʱ�䣬�ɻ�Զ�����
	t_uint32     userInfoEx2;         //��չ�ֶ�2������GM����֮����ʱЧ����zxd�����20100805
	t_int32		 bTrader;		      //�����ж��ǲ�������
	bool         bIsCanTransBack;     //�Ƿ�ϻ���      
	t_int64	     i64ContestScore;     //�����ɼ�
	t_int32	     iContestCount;       //��������
	t_int64      timeLeft;            //����ʣ��ʱ��
	t_int64      i64TimeEnd;          //��������ʱ��
	t_int64      i64TimeStart;        //������ʼʱ��
	t_int32		 iRankNum;		      //��������
};

struct TLoginResult
{
	t_uint32     dwGamePower;             //�û�Ȩ�� 4
	t_uint32     dwMasterPower;           //����Ȩ�� 8

	t_uint32     dwRoomRule;              //���ù��� 12
	t_uint32     uLessPoint;              //���پ���ֵ 16

	t_uint32     uMaxPoint;               //��ྭ��ֵ 20
	
	//���룬����4�ֽ�
	TUserData    UserInfoStruct;	      //�û���Ϣ   20+808=828

	bool         rm_bISOpen;    	      //�Զ�����ר�� (rm = Receive Money)
	
	//���룬����7�ֽ�
	t_int64      rm_i64Money;             //�Զ�����ר��,?

	t_int64      rm_i64MinMoney;          //�Զ�����ר��,?

	t_int32      rm_iCount;               //�Զ�����ר��,?
	t_int32      rm_iTotal;               //�Զ�����ר��,?

	t_int32      rm_iTime;                //�Զ�����ר��,?
	t_int32      rm_iResultCode;          //�Զ�����ר��,?

	//[-- 
	t_int32      rm_iLessPoint;           //�Զ�����ר��,?
	char         mbi_szMatchDesp[200];    //��������	(mbi = Match Base Info)
	t_int32      mbi_nReportCounts;       //��������
	//--] 208�ֽ� ������벹��

	t_int32      mbi_nMatchActors;        //��������	
	t_int32      mbi_nScoreDiff;          //�����ϼ����Ļ��ֲ�,Ĭ����һ��

	t_int32      mbi_nPeopleLimit;        //������������	
	bool         mbi_ISBegin;             //��δ���Ƿ��Կ��� 
	bool         mbi_ISMatchEnd;          //�����Ƿ��Ѿ�����
	
	//���� ����2�ֽ�
	t_int32      nVirtualUser;            //��¼����ʱ��ʱ��ȡ�����������
	t_int32      nPresentCoinNum;         //���ͽ������

	t_int32	     iContestID;              //����ר��,����ID
	t_int32      iLowCount;               //����ר��,?

	t_int64	     i64Chip;                 //����ר��,?

	t_int64	     i64TimeStart;            //������ʼʱ��

	t_int64	     i64TimeEnd;              //��������ʱ��

	t_int64	     i64LowChip;              //������̭������

	t_int32	     iTimeout;                //��������ʱ��
	t_int32	     iBasePoint;              //����׷�
};

#define OFFSET(struc,e) ((int)(&((struc*)0)->e))
void TestStruct(){
	printf("********************���� Struct**************************\n");
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
	printf("********************���� ���߳�************************** %x\n",std::this_thread::get_id());

	g_t.mutex = &g_mutex;
	g_t.step = 4;

	std::thread add = std::thread(AddThread, std::ref(g_t), 2);
	std::thread sub = std::thread(SubThread, &g_t);

	//�����߳�
	add.detach();
	sub.detach();

	//�ȴ����߳̽���
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
		�ڴ��ַ��	С ---->  ��

		���ݸ�λ���ڸ�λ��ַ 0x9f < 0x8f  ���� < С��
		���ݸ�λ���ڵ�λ��ַ 0x8f > 0x9f  ���� > ���
	*/
	unsigned char big0 = p[0];
	printf("С�� %#x : %x,%#X,%x,%x\n", big0, p[0] & 0xff, p[1] & 0xff, pU[2], p[3]);

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


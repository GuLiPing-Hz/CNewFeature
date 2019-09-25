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
#include <mutex>
#include <sstream>

class C11 {

public:
	C11() {

	}

	//�ɱ䳤ģ���������
	template<typename... Tx>
	static void test1(const char* format, Tx... param) {
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
	void setFunc(std::function<void()> lam) {
		mFunc = lam;
	}

	void test2() {
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
	void lrValue(int &a) {//��ֵ����
		std::cout << "left value " << a << std::endl;
	}
	void lrValue(int &&a) {//��ֵ����
		std::cout << "right value " << a << std::endl;
		std::cout << "right value " << ++a << std::endl;//�ɱ�

		lrValue(++a);//�������ã�֤��a�������Ѿ�����ֵ��
	}
};

void TestBind() {
	printf("******************���� std::bind****************************\n");
	//std::bind����
	int n = 7;
	// (_1 and _2 are from std::placeholders, and represent future
	// arguments that will be passed to f1)
	/*
	std::ref ���ڰ�װ�����ô��ݵ�ֵ��
	std::cref ���ڰ�װ��const���ô��ݵ�ֵ��
	*/
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

void TestTemplate() {

	printf("*********************�ɱ䳤ģ�����*************************\n");
	//�ɱ䳤ģ�����
	C11::test1("[%s] %d + %d = %d\n", "add", 1, 2, 3);
	const char* s1 = "123456";

	auto lam = [s1]()->void {
		printf("%s\n", s1);
	};
	C11 obj;
	obj.setFunc(lam);
	obj.test2();
}

void TestMove() {
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

void TestRightValue() {
	printf("********************���� ��ֵ����**************************\n");
	Value va;
	int val = 10;
	va.lrValue(val);//��ֵ����
	/*
	���û�ж��� lrValue(int && n)//���ǽ�����ʹ�������д��
	*/
	va.lrValue(10);//��ֵ����
}

void TestXor() {
	printf("********************���� ���**************************\n");
	char p1[] = "123abc";
	const char* key = "xxxxx";
	std::cout << "ԭ�����ַ���:" << p1 << std::endl;

	int keyLen = strlen(key);
	for (unsigned int i = 0; i < strlen(p1); i++) {
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "���1�ε��ַ���:" << p1 << std::endl;

	for (unsigned int i = 0; i < strlen(p1); i++) {
		p1[i] = p1[i] ^ key[i%keyLen];
	}
	std::cout << "���2�ε��ַ���:" << p1 << std::endl;

	char* p = new char[100];
	std::auto_ptr<char>pA(p);
}

void TestVector() {
	printf("********************���� Vector**************************\n");
	std::vector<unsigned char> vect;
	std::string str;
	vect.assign(5, 'a');
	unsigned char* p = vect.data();
	str.insert(0, (const char*)p, vect.size());
	printf("p=%s,size=%d\n", str.c_str(), vect.size());

	const char* p1 = "bcdef";
	vect.insert(vect.end(), (const unsigned char*)p1, (const unsigned char*)p1 + strlen(p1));
	p = vect.data();
	str.insert(str.begin(), vect.begin(), vect.end());
	printf("p=%s,size=%d\n", str.c_str(), vect.size());
}

struct BufferUnit {
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

	BufferUnit() { memset(&data, 0, sizeof(data)); }
	virtual ~BufferUnit() {}

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

void printUnion(BufferUnit& bu) {
	printf("union %c(%d),%d,%d,%lld,%f\n", bu.data.c, bu.data.c, bu.data.s, bu.data.i
		, bu.data.ll, bu.data.f);
}

void TestUnion() {
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
void TestStruct() {
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

struct ThreadT {
	std::mutex* mutex;
	int step;
};

void AddThread(ThreadT& t, int step) {

	for (int i = 0; i < 10; i++)
	{
		std::lock_guard<std::mutex> guard(*t.mutex);
		std::stringstream ss;
		ss << std::this_thread::get_id();
		printf("********************************************%s \n", ss.str().c_str());
		printf("AddThread1 ,%d, %d\n", g_Cnt, t.step);
		g_Cnt += step;
		printf("AddThread2 ,%d\n", g_Cnt);
		Sleep(500);
	}

}

void SubThread(void* v) {
	ThreadT* t = (ThreadT*)v;
	for (int i = 0; i < 10; i++)
	{
		std::lock_guard<std::mutex> guard(*t->mutex);
		std::stringstream ss;
		ss << std::this_thread::get_id();
		printf("********************************************%s \n", ss.str().c_str());
		printf("SubThread1 ,%d\n", g_Cnt);
		g_Cnt -= 1;
		printf("SubThread2 ,%d\n", g_Cnt);
		Sleep(500);
		//std::this_thread::sleep_until()
	}
}

std::mutex g_mutex;
ThreadT g_t;

void TestThread() {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	printf("********************���� ���߳�************************%s \n", ss.str().c_str());

	g_t.mutex = &g_mutex;
	g_t.step = 4;

	std::thread add = std::thread(AddThread, std::ref(g_t), 2);
	std::thread sub = std::thread(SubThread, &g_t);

	//�����߳�
	add.detach();
	sub.detach();

	Sleep(3000);

	//�ȴ����߳̽���
	//add.join();
	//sub.join();
}

/*
size ���ֽڸ���
nmemb ����
*/
size_t my_write_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	printf("[%d]%p\n", size*nmemb, ptr);
	return fwrite(ptr, size, nmemb, (FILE*)stream);
}

#include "curl/curl.h"

int my_progress_func(char *progress_data,
	double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow)
{
	printf("%s %g / %g (%g %%)\n", progress_data, d, t, d*100.0 / t);
	return 0;
}

#include <unordered_map>
#include <set>

/*
* Download a HTTP file and upload an FTP file simultaneously.
*/

#define HANDLECOUNT 2    
#define HTTP_HANDLE 0    
#define FTP_HANDLE 1     

#pragma comment(lib,"ws2_32.lib")
int TestCurlMulti(void)
{
	CURL *handles[HANDLECOUNT];
	CURLM *multi_handle;

	int still_running; /* keep number of running handles */
	int i;

	CURLMsg *msg; /* for picking up messages with the transfer status */
	int msgs_left; /* how many messages are left */

	/* Allocate one CURL handle per transfer */
	for (i = 0; i < HANDLECOUNT; i++)
		handles[i] = curl_easy_init();

	/* set the options (I left out a few, you'll get the point anyway) */
	curl_easy_setopt(handles[HTTP_HANDLE], CURLOPT_URL, "http://example.com");

	curl_easy_setopt(handles[FTP_HANDLE], CURLOPT_URL, "ftp://example.com");
	curl_easy_setopt(handles[FTP_HANDLE], CURLOPT_UPLOAD, 1L);

	/* init a multi stack */
	multi_handle = curl_multi_init();

	/* add the individual transfers */
	for (i = 0; i < HANDLECOUNT; i++)
		curl_multi_add_handle(multi_handle, handles[i]);

	/* we start some action by calling perform right away */
	curl_multi_perform(multi_handle, &still_running);

	do {
		struct timeval timeout;
		int rc; /* select() return code */
		CURLMcode mc; /* curl_multi_fdset() return code */

		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		int maxfd = -1;

		long curl_timeo = -1;

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		/* set a suitable timeout to play around with */
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		curl_multi_timeout(multi_handle, &curl_timeo);
		if (curl_timeo >= 0) {
			timeout.tv_sec = curl_timeo / 1000;
			if (timeout.tv_sec > 1)
				timeout.tv_sec = 1;
			else
				timeout.tv_usec = (curl_timeo % 1000) * 1000;
		}

		/* get file descriptors from the transfers */
		mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

		if (mc != CURLM_OK) {
			fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
			break;
		}

		/* On success the value of maxfd is guaranteed to be >= -1. We call
		select(maxfd + 1, ...); specially in case of (maxfd == -1) there are
		no fds ready yet so we call select(0, ...) --or Sleep() on Windows--
		to sleep 100ms, which is the minimum suggested value in the
		curl_multi_fdset() doc. */

		if (maxfd == -1) {
#ifdef _WIN32
			Sleep(100);
			rc = 0;
#else
			/* Portable sleep for platforms other than Windows. */
			struct timeval wait = { 0, 100 * 1000 }; /* 100ms */
			rc = select(0, NULL, NULL, NULL, &wait);
#endif
		}
		else {
			/* Note that on some platforms 'timeout' may be modified by select().
			If you need access to the original value save a copy beforehand. */
			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
		}

		switch (rc) {
		case -1:
			/* select error */
			break;
		case 0: /* timeout */
		default: /* action */
			curl_multi_perform(multi_handle, &still_running);
			break;
		}
	} while (still_running);

	/* See how the transfers went */
	while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
		if (msg->msg == CURLMSG_DONE) {
			int idx, found = 0;

			/* Find out which handle this message is about */
			for (idx = 0; idx < HANDLECOUNT; idx++) {
				found = (msg->easy_handle == handles[idx]);
				if (found)
					break;
			}

			switch (idx) {
			case HTTP_HANDLE:
				printf("HTTP transfer completed with status %d\n", msg->data.result);
				break;
			case FTP_HANDLE:
				printf("FTP transfer completed with status %d\n", msg->data.result);
				break;
			}
		}
	}

	curl_multi_cleanup(multi_handle);

	/* Free the CURL handles */
	for (i = 0; i < HANDLECOUNT; i++)
		curl_easy_cleanup(handles[i]);

	return 0;
}

void TestCurlLocalCA() {
	/* some requirements for this to work:
	1.   set pCertFile to the file with the client certificate
	2.   if the key is passphrase protected, set pPassphrase to the
	passphrase you use
	3.   if you are using a crypto engine:
	3.1. set a #define USE_ENGINE
	3.2. set pEngine to the name of the crypto engine you use
	3.3. set pKeyName to the key identifier you want to use
	4.   if you don't use a crypto engine:
	4.1. set pKeyName to the file name of your client key
	4.2. if the format of the key file is DER, set pKeyType to "DER"

	!! verify of the server certificate is not implemented here !!

	**** This example only works with libcurl 7.9.3 and later! ****

	*/

	CURL *curl;
	CURLcode res;
	FILE *headerfile;
	const char *pPassphrase = NULL;

	static const char *pCertFile = "testcert.pem";
	static const char *pCACertFile = "cacert.pem";
	static const char *pHeaderFile = "dumpit";

	const char *pKeyName;
	const char *pKeyType;

	const char *pEngine;

#ifdef USE_ENGINE
	pKeyName = "rsa_test";
	pKeyType = "ENG";
	pEngine = "chil";            /* for nChiper HSM... */
#else
	pKeyName = "testkey.pem";
	pKeyType = "PEM";
	pEngine = NULL;
#endif

	headerfile = fopen(pHeaderFile, "wb");

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl) {
		/* what call to write: */
		curl_easy_setopt(curl, CURLOPT_URL, "HTTPS://your.favourite.ssl.site");
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);

		do { /* dummy loop, just to break out from */
			if (pEngine) {
				/* use crypto engine */
				if (curl_easy_setopt(curl, CURLOPT_SSLENGINE, pEngine) != CURLE_OK) {
					/* load the crypto engine */
					fprintf(stderr, "can't set crypto engine\n");
					break;
				}
				if (curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L) != CURLE_OK) {
					/* set the crypto engine as default */
					/* only needed for the first time you load
					a engine in a curl object... */
					fprintf(stderr, "can't set crypto engine as default\n");
					break;
				}
			}
			/* cert is stored PEM coded in file... */
			/* since PEM is default, we needn't set it for PEM */
			curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");

			/* set the cert for client authentication */
			curl_easy_setopt(curl, CURLOPT_SSLCERT, pCertFile);

			/* sorry, for engine we must set the passphrase
			(if the key has one...) */
			if (pPassphrase)
				curl_easy_setopt(curl, CURLOPT_KEYPASSWD, pPassphrase);

			/* if we use a key stored in a crypto engine,
			we must set the key type to "ENG" */
			curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, pKeyType);

			/* set the private key (file or ID in engine) */
			curl_easy_setopt(curl, CURLOPT_SSLKEY, pKeyName);

			/* set the file with the certs vaildating the server */
			curl_easy_setopt(curl, CURLOPT_CAINFO, pCACertFile);

			/* disconnect if we can't validate server's cert */
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

			/* we are done... */
		} while (0);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return;
}

void TestCurl() {
	printf("********************���� cURL**************************\n");

	CURL *curl;
	CURLcode res;
	FILE *outfile;
	char *url = "https://upload-images.jianshu.io/upload_images/5109294-2fbc423082a3ebfe.png";
	//char *url = "http://bpic.588ku.com/element_origin_min_pic/17/12/25/5296828180d0869d34634a881acad6a1.jpg";
	char *progress_data = "* ";

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl)
	{
		outfile = fopen("xx.png", "wb");

		curl_easy_setopt(curl, CURLOPT_URL, url);

#define SKIP_PEER_VERIFICATION
#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);//��֤����֤��
		//curl_easy_setopt(curl, CURLOPT_CAPATH, "xxx.xx");//���ñ���֤��·��
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_progress_func);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress_data);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %d(%s)\n", res, curl_easy_strerror(res));

		fflush(outfile);
		fclose(outfile);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
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

/*ȡ�������Ĺ�Լ��*/
int getGCD(int p, int q) {//շת�����
	if (p < q) {//λ�û���
		p = p ^ q;
		q = p ^ q;
		p = p ^ q;
	}

	do {
		int mod = p % q;
		if (mod == 0) {
			return q;
		}

		p = q;
		q = mod;
	} while (true);
}

typedef struct {
	int flag;//1��ʾ·��0��ʾǽ
	int res;//��Ʒ
	int resPos;//��Ʒ����ǽ�ϵ�λ�ã�0��ʾ��߻����ϱߣ�1��ʾ�ұ߻����±�
}MazeItem;
void MakeMaze() {

}
void TestMaze(int l) {
	std::vector<MazeItem> vMaze;
	vMaze.resize(l * l);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int g1 = getGCD(8850, 12345678);

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

	const char* str = "0123456789abcd";
	int count = 5;
	printf("��ʽ���ַ�������[%%.%ds] : %.*s", count, count, str);

	TestXor();
	TestBind();
	TestMove();
	TestRightValue();
	TestTemplate();
	TestVector();
	TestUnion();
	TestStruct();
	//TestThread();

	TestCurl();
	//TestCurlLocalCA();

	TestMaze(1);

	getchar();

	return 0;
}


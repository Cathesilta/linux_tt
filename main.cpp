//#include <cstdio>
//
#include <thread>
#include <iostream>
using namespace std;

#include "redis/redis_test/redis_hash_test.h"
#include "redis/redis_test/redis_string_test.h"

#include <sys/time.h>

inline std::string get_time_for_ms()
{
	timeval tmval;
	gettimeofday(&tmval, NULL);

	struct tm tml;
	localtime_r(&tmval.tv_sec, &tml);

	char buffer[30] = { 0 };
	snprintf(buffer, 30, "%04d-%02d-%02d %02d:%02d:%02d:%03ld", tml.tm_year + 1900, tml.tm_mon + 1, tml.tm_mday, tml.tm_hour, tml.tm_min, tml.tm_sec, tmval.tv_usec / 1000);

	return std::string(buffer);
}

void log_func(int level, const std::string& msg)
{
	cout << get_time_for_ms() << "	level:" << level << " " << msg << endl;
}
////
////#define KEY_LEN 32
////#define KEY_MAX_LEN 256
////#define CIPER_SUITE_MAX_LEN 4
////typedef struct stru_ouyu_catch stru_ouyu_catch;
////struct stru_ouyu_catch
////{
////	char 							ouyu_number[20 + 1];			//����
////	char							string_id[KEY_LEN + 1];			//����id
////	char							session_key[KEY_MAX_LEN + 1];		//�Ự
////	unsigned int			session_key_len;
////	char							storage_key[KEY_MAX_LEN + 1];		//�洢
////	unsigned int			storage_key_len;
////	char							overlap;						//�ڲ�Խ���
////	char							ciper_suite_id[CIPER_SUITE_MAX_LEN + 1];					//�����׼�
////	unsigned int			ciper_suite_id_len;
////	time_t						gmt;							//��������ʱ���
////	char							client_hello;					//�ͻ��˼����˵�
////	void* ext_buff;						//��չ�ڴ�
////};
////
////
////void gerate_data()
////{
////
////}
//



int main()
{

	//{
	//	redis_hash_op_base rhob(log_func);
	//	if (false == rhob.init_db())
	//	{
	//		cout << "init db failed..." << endl;
	//	}

	//	hash_test(rhob);
	//}

	redis_string_op_base rhob(log_func);
	if (false == rhob.init_db())
	{
		cout << "init db failed..." << endl;
	}

	string_test(rhob);
//
//	//��һ�ֲ��ԣ����߳�д��1000 0000 �����ݣ�����ȡ
//
//	//�ڶ��ֲ��ԣ������ͬ��д��(������)
//
//	return 0;
}
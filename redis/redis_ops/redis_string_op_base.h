#ifndef redis_string_op_base_h__
#define redis_string_op_base_h__
/*!
 * \file redis_string_op_base.h
 * \date 2019/05/21 11:28
 *
 * \author JC
 * Contact: jiangchun2014@hotmail.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/

#include "../redis_base/redis_base.h"

#include <map>
#include <vector>

class redis_string_op_base
	: public redis_base
{
public:
	using redis_base::init_db;
	using redis_base::disconnect_db;
	using redis_base::select_page;
	using redis_base::log_msg;

	using redis_base::exec_command;
public:
	redis_string_op_base(redis_log_func_t func = nullptr, std::string con_host = "127.0.0.1", short con_port = 6379, std::string password = "", int con_timeout = 0);
	~redis_string_op_base();

	//���ַ���ֵ value ������ key ��
	bool set(const std::string& key, const std::string& value, const std::string& extern_command = "", long long expire_time = 0);
	//�������ڵ�λΪseconds
	bool set_ex(const std::string& key, const std::string& value, long long seconds);
	//�������ڵ�λΪmilliseconds
	bool set_px(const std::string& key, const std::string& value, long long milliseconds);
	//�������������óɹ�
	bool set_nx(const std::string& key, const std::string& value);
	//�����������óɹ�
	bool set_xx(const std::string& key, const std::string& value);

	//������� key ��������ַ���ֵ��
	std::string get(const std::string& key);
	//���� key ��ֵ��Ϊ value �� �����ؼ� key �ڱ�����֮ǰ�ľ�ֵ��
	std::string getset(const std::string& key, const std::string& new_value);

	//���ؼ� key ������ַ���ֵ�ĳ��ȡ�
	long long strlen(const std::string& key);

	//�� value ׷�ӵ��� key ����ֵ��ĩβ��
	long long append(const std::string& key, const std::string& value);
	//��ƫ���� offset ��ʼ�� �� value ������д(overwrite)�� key ������ַ���ֵ��
	long long setrange(const std::string& key, long long offset, const std::string& value);
	//���ؼ� key ������ַ���ֵ��ָ�����֣� �ַ����Ľ�ȡ��Χ�� start �� end ����ƫ�������� (���� start �� end ����)��
	std::string getrange(const std::string& key, long long start, long long end);
	
	//Ϊ�� key ���������ֵ����һ��
	long long incr(const std::string& key);

	long long incrby(const std::string& key, long long increment);

	float incrbyfloat(const std::string& key, float increment);

	long long decr(const std::string& key);

	long long decrby(const std::string& key, long long increment);

	//ͬʱΪ���������ֵ��
	bool mset(const std::map <std::string,std::string> &kvs);
	//���ҽ������и�������������ʱ�� Ϊ���и���������ֵ��
	bool mset_nx(const std::map <std::string, std::string>& kvs);
	//���ظ�����һ�������ַ�������ֵ��
	std::map<std::string, std::string > mget(const std::vector<std::string>& keys);

};

#endif // redis_string_op_base_h__

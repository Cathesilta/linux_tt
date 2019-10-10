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

	//将字符串值 value 关联到 key 。
	bool set(const std::string& key, const std::string& value, const std::string& extern_command = "", long long expire_time = 0);
	//生命周期单位为seconds
	bool set_ex(const std::string& key, const std::string& value, long long seconds);
	//生命周期单位为milliseconds
	bool set_px(const std::string& key, const std::string& value, long long milliseconds);
	//键不存在则设置成功
	bool set_nx(const std::string& key, const std::string& value);
	//键存在则设置成功
	bool set_xx(const std::string& key, const std::string& value);

	//返回与键 key 相关联的字符串值。
	std::string get(const std::string& key);
	//将键 key 的值设为 value ， 并返回键 key 在被设置之前的旧值。
	std::string getset(const std::string& key, const std::string& new_value);

	//返回键 key 储存的字符串值的长度。
	long long strlen(const std::string& key);

	//把 value 追加到键 key 现有值的末尾。
	long long append(const std::string& key, const std::string& value);
	//从偏移量 offset 开始， 用 value 参数覆写(overwrite)键 key 储存的字符串值。
	long long setrange(const std::string& key, long long offset, const std::string& value);
	//返回键 key 储存的字符串值的指定部分， 字符串的截取范围由 start 和 end 两个偏移量决定 (包括 start 和 end 在内)。
	std::string getrange(const std::string& key, long long start, long long end);
	
	//为键 key 储存的数字值加上一。
	long long incr(const std::string& key);

	long long incrby(const std::string& key, long long increment);

	float incrbyfloat(const std::string& key, float increment);

	long long decr(const std::string& key);

	long long decrby(const std::string& key, long long increment);

	//同时为多个键设置值。
	bool mset(const std::map <std::string,std::string> &kvs);
	//当且仅当所有给定键都不存在时， 为所有给定键设置值。
	bool mset_nx(const std::map <std::string, std::string>& kvs);
	//返回给定的一个或多个字符串键的值。
	std::map<std::string, std::string > mget(const std::vector<std::string>& keys);

};

#endif // redis_string_op_base_h__

#ifndef redis_hash_op_base_h__
#define redis_hash_op_base_h__
/*!
 * \file redis_hash_op_base.h
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

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

class redis_hash_op_base
	: public redis_base
{
public:
	using redis_base::init_db;
	using redis_base::disconnect_db;
	using redis_base::select_page;
	using redis_base::log_msg;

	using redis_base::exec_command;

public:
	redis_hash_op_base(redis_log_func_t func = nullptr, std::string con_host = "127.0.0.1", short con_port = 6379, std::string password = "", int con_timeout = 0);
	~redis_hash_op_base();

	//将哈希表 hash 中域 field 的值设置为 value 。
	void hset(const std::string& key, const std::string& field, const std::string& value);

	//当且仅当域 field 尚未存在于哈希表的情况下， 将它的值设置为 value 。
	bool hset_nx(const std::string& key, const std::string& field, const std::string& value);

	//返回哈希表中给定域的值。
	std::string get(const std::string& key, const std::string& field);

	//检查给定域 field 是否存在于哈希表 hash 当中。
	bool hexists(const std::string& key, const std::string& field);

	//删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略。
	bool hdel(const std::string& key, const std::string& field);
	void hdel(const std::string& key, const std::vector<std::string>& fields);

	//返回哈希表 key 中域的数量。
	long long hlen(const std::string& key);

	//返回哈希表 key 中， 与给定域 field 相关联的值的字符串长度
	long long hstrlen(const std::string& key, const std::string& field);

	//为哈希表 key 中的域 field 的值加上增量 increment 。
	long long hincrby(const std::string& key, const std::string& field, long long increment);

	//为哈希表 key 中的域 field 加上浮点数增量 increment 。
	float hincrbyfloat(const std::string& key, const std::string& field, float increment);

	//同时将多个 field-value (域-值)对设置到哈希表 key 中。
	bool hmset(const std::string& key, const std::map<std::string, std::string>& f_v);

	//返回哈希表 key 中，一个或多个给定域的值。
	std::unordered_map <std::string, std::string > hmget(const std::string& key, const std::vector<std::string>& fields);

	//返回哈希表 key 中的所有域。
	bool hkeys(const std::string& key, std::vector<std::string>& fields);

	//返回哈希表 key 中所有域的值。
	bool hvals(const std::string& key, std::vector<std::string>& values);

	//返回哈希表 key 中，所有的域和值。
	bool hgetall(const std::string& key, std::unordered_map<std::string, std::string>& f_v);
};

#endif // redis_hash_op_base_h__

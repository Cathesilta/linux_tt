#ifndef redis_set_op_base_h__
#define redis_set_op_base_h__

/*!
 * \file redis_set_op_base.h
 * \date 2019/05/21 11:27
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

#include <vector>

class redis_set_op_base
	: public redis_base
{
public:
	using redis_base::init_db;
	using redis_base::disconnect_db;
	using redis_base::select_page;
	using redis_base::log_msg;

	using redis_base::exec_command;
public:
	redis_set_op_base(redis_log_func_t func = nullptr, std::string con_host = "127.0.0.1", short con_port = 6379, std::string password = "", int con_timeout = 0);
	~redis_set_op_base();

	//将一个或多个 member 元素加入到集合 key 当中，已经存在于集合的 member 元素将被忽略。
	bool sadd(const std::string& key, const std::string& member);
	bool sadd(const std::string& key, const std::vector<std::string> &members);

	//判断 member 元素是否集合 key 的成员。
	bool sismember(const std::string& key, const std::string& member);

	//移除并返回集合中的一个随机元素。
	std::string spop(const std::string& key);

	//只提供 key 参数时，返回一个元素；如果集合为空，返回 nil 。 如果提供了 count 参数，那么返回一个数组；如果集合为空，返回空数组。
	std::string srandmember(const std::string& key);
	std::vector<std::string> srandmember(const std::string& key, long long count);

	//移除集合 key 中的一个或多个 member 元素，不存在的 member 元素会被忽略
	void srem(const std::string& key, const std::string& val);
	void srem(const std::string& key, const std::vector<std::string>& vals);

	//将 member 元素从 source 集合移动到 destination 集合。
	void smove(const std::string& source, const std::string& destination, const std::string& member);

};

#endif // redis_set_op_base_h__


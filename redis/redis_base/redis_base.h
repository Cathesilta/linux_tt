#ifndef redis_base_h__
#define redis_base_h__

/*!
 * \class redis_base
 *
 * \brief 
 *
 * \author JC
 * \date 四月 2019
 */

#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>

#include "redis_type.h"

extern "C" {
#include <hiredis/hiredis.h>
}



class redis_base
{
public:
	redis_base(redis_log_func_t func = nullptr, std::string con_host = "127.0.0.1", short con_port = 6379, std::string password = "", int con_timeout = 0);
	~redis_base();


	bool init_db();

	void disconnect_db();

	bool reconnect_db();

	bool select_page(int page_num);


public:
	//检查给定 key 是否存在。
	bool exists(const std::string& key);

	long long ttl(const std::string &key);

	long long pttl(const std::string& key);

protected:

	void log_msg(int log_level, const std::string& msg);

	//执行redis命令行指令
	redisReply* exec_command(const std::string& command);

	//处理返回值
	std::string get_reply_string(redisReply *reply);
	long long get_reply_integer(redisReply *reply);
	bool get_reply_array(redisReply *reply, std::vector<redisReply*>& results);

	//分布提交
	bool multi_is_QUEUED(redisReply *reply);
	//判断执行结果是否为ok
	bool exec_command_is_OK(redisReply *reply);

	//安全释放结果集
	void safe_free_reply(redisReply **reply_ptr) { if (*reply_ptr) { freeReplyObject(*reply_ptr); *reply_ptr = NULL; } }

private:
	void handle_dbop_err(void);
	std::string get_connect_err_msg() { return "code is:" + std::to_string(redis_con->err) + ",msg is:" + redis_con->errstr; };
private:
	bool connect_db();
	bool auth_db();
	bool ping_pong();

protected:
	redisContext* redis_con;
	redis_log_func_t log_f;
protected:
	std::string domain;
	short port;
	std::string pwd;
	int timeout;
};

#endif // redis_base_h__


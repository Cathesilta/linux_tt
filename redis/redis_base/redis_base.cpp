#include "redis_base.h"

#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#endif // _WIN32

#ifdef _WIN32
//本地时间 2014-9-12 10:49:53:209
inline std::string get_time_for_ms()
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);

	char buffer[30] = { 0 };
	sprintf_s(buffer, "%04d-%02d-%02d %02d:%02d:%02d:%03d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);

	return std::string(buffer);
}


#else
//本地时间 2014-9-12 10:49:53:209
inline std::string get_time_for_ms()
{
	timeval tmval;
	gettimeofday(&tmval, NULL);

	struct tm tml;
	localtime_r(&tmval.tv_sec,& tml);

	char buffer[30] = { 0 };
	snprintf(buffer, 30, "%04d-%02d-%02d %02d:%02d:%02d:%03ld", tml.tm_year + 1900, tml.tm_mon + 1, tml.tm_mday, tml.tm_hour, tml.tm_min, tml.tm_sec, tmval.tv_usec / 1000);

	return std::string(buffer);
}
#endif // _WIN32

#define THIS_CLASS_FUNC CLASS_FUNC("redis_base")

redis_base::redis_base(redis_log_func_t func /*= nullptr*/, std::string con_host /*= "127.0.0.1"*/, short con_port /*= 6379*/, std::string password /*= ""*/, int con_timeout /*= 0*/)
	: log_f(func), domain(con_host), port(con_port), pwd(password), timeout(con_timeout)
{

}

redis_base::~redis_base()
{
	disconnect_db();
}


bool redis_base::init_db()
{
	bool result = false;
	do 
	{
		//1、连接
		if (false == connect_db())
		{
			break;
		}
		//2、鉴权\\第一次要设定密码(禁止其他人访问)
		if (false == auth_db())
		{
			break;
		}
		//3、发送pingpong
		if (true != ping_pong())
		{
			break;
		}

		result = true;
	} while (false);

	return result;
}

bool redis_base::connect_db()
{
	if (0 == timeout)
	{
		redis_con = redisConnect(domain.c_str(), port);
	}
	else
	{
		struct timeval time_out = { timeout,0 };
		redis_con = redisConnectWithTimeout(domain.c_str(), port, time_out);
	}

	if (redis_con == NULL || redis_con->err) {
		if (redis_con) {
			log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + std::string("redis connect failed, ") 
				+ get_connect_err_msg() + ", domain is:" + domain);
		}
		else {
			log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + 
				"redis connect failed, Can't allocate redis context, domain is:" + domain);
		}
		return false;
	}

	log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "open redis success, domain is:" + domain);
	return true;
}


void redis_base::disconnect_db()
{
	if (redis_con)
	{
		redisFree(redis_con);
		redis_con = nullptr;
	}
}

bool redis_base::reconnect_db(void)
{
	log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "reconnect redis, domain is:" + domain);

	if (redis_con)
	{
		disconnect_db();
	}

	return init_db();
}

bool redis_base::select_page(int page_num)
{
	if (nullptr == redis_con)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC +"redis not connect, domain is:" + domain);
		return false;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SELECT page, domain is:" + domain);

	redisReply *reply = exec_command("select " + page_num);
	if (0 == exec_command_is_OK(reply))
	{
		return false;
	}

	return false;
}

bool redis_base::exists(const std::string& key)
{
	redisReply* reply = exec_command("EXISTS " + key);
	long long exist_f = get_reply_integer(reply);

	safe_free_reply(&reply);

	if (1 == exist_f)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " exist");
		return true;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " not exist");
	return false;
}

long long redis_base::ttl(const std::string& key)
{
	redisReply* reply = exec_command("TTL " + key);
	long long ttl_value =  get_reply_integer(reply);

	safe_free_reply(&reply);
	if (-1 == ttl_value)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " not TTL");
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",TTL is:" + std::to_string(ttl_value));
	}


	return ttl_value;
}

long long redis_base::pttl(const std::string& key)
{
	redisReply* reply = exec_command("PTTL " + key);
	long long pttl_value = get_reply_integer(reply);

	safe_free_reply(&reply);
	if (-1 == pttl_value)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " not PTTL");
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",PTTL is:" + std::to_string(pttl_value));
	}


	return pttl_value;
}

void redis_base::log_msg(int log_level, const std::string& msg)
{
	if (log_f)
	{
		return log_f(log_level, msg);
	}
}

redisReply* redis_base::exec_command(const std::string& command)
{
	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "exec_command: " + command + ", domain is:" + domain);
	redisReply *reply = (redisReply *)redisCommand(redis_con,command.c_str());
	if (REDIS_REPLY_ERROR == reply->type)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, "exec_command failed: " + command + ",err info is:" 
			+ std::string(reply->str) + ", domain is:" + domain);
		handle_dbop_err();
		return nullptr;
	}

	return reply;
}

void redis_base::handle_dbop_err(void)
{
	//判断连接是否异常
	if (REDIS_OK != redis_con->err)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "redis connection error, " 
			+ get_connect_err_msg() + ", domain is:" + domain);
		reconnect_db();
	}
}

std::string redis_base::get_reply_string(redisReply *reply)
{
	if (NULL != reply)
	{
		if (REDIS_REPLY_STRING == reply->type)
		{
			return std::string(reply->str, reply->len);
		}
		else if (REDIS_REPLY_NIL == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is nil, domain is:" + domain);
		}
		else if (REDIS_REPLY_ERROR == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is err, err info:" 
				+ std::string(reply->str, reply->len) + ", domain is:" + domain);
		}
		else
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is not string, type is:" 
				+ std::to_string(reply->type) + ", domain is:" + domain);
		}
	}
	
	return "";
}

long long redis_base::get_reply_integer(redisReply *reply)
{
	if (NULL != reply)
	{
		if (REDIS_REPLY_INTEGER == reply->type)
		{
			return (long long)(reply->integer);
		}
		else if (REDIS_REPLY_NIL == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is nil, domain is:" + domain);
		}
		else if (REDIS_REPLY_ERROR == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is err, err info:"
				+ std::string(reply->str, reply->len) + ", domain is:" + domain);
		}
		else
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is not integer, type is:"
				+ std::to_string(reply->type) + ", domain is:" + domain);
		}
	}

	return -1;
}

bool redis_base::get_reply_array(redisReply *reply, std::vector<redisReply*>& results)
{
	if (NULL != reply)
	{
		if (REDIS_REPLY_ARRAY == reply->type)
		{
			size_t num = (size_t)reply->elements;
			log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "reply array size is:" 
				+ std::to_string(num) + ", domain is:" + domain);
			for (size_t i = 0; i < num; ++ i)
			{
				results.push_back(reply->element[i]);
			}

			return true;
		}
		else if (REDIS_REPLY_NIL == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is nil, domain is:" + domain);
		}
		else if (REDIS_REPLY_ERROR == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is err, err info:"
				+ std::string(reply->str, reply->len) + ", domain is:" + domain);
		}
		else
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is not array, type is:"
				+ std::to_string(reply->type) + ", domain is:" + domain);
		}
	}

	handle_dbop_err();
	return false;
}

bool redis_base::multi_is_QUEUED(redisReply *reply)
{
	if (NULL != reply)
	{
		if (REDIS_REPLY_STATUS == reply->type && 0 == memcmp(reply->str, "QUEUED", 6))
		{
			return true;
		}
		else if (REDIS_REPLY_NIL == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is nil, domain is:" + domain);
		}
		else if (REDIS_REPLY_ERROR == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is err, err info:"
				+ std::string(reply->str, reply->len) + ", domain is:" + domain);
		}
		else
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is not array, type is:"
				+ std::to_string(reply->type) + ", domain is:" + domain);
		}
	}

	handle_dbop_err();
	return false;
}

bool redis_base::exec_command_is_OK(redisReply *reply)
{
	if (nullptr == redis_con)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "exec command failed," 
			+ std::string(reply->str, reply->len) + " redis not connect, domain is:" + domain);
		return false;
	}
	if (NULL != reply)
	{
		if (REDIS_REPLY_STATUS == reply->type 
			&& 0 == memcmp(reply->str, "OK", 2))
		{
			return true;
		}
		else if (REDIS_REPLY_NIL == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is nil, domain is:" + domain);
		}
		else if (REDIS_REPLY_ERROR == reply->type)
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is err, err info:"
				+ std::string(reply->str, reply->len) + ", domain is:" + domain);
		}
		else
		{
			log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "reply is not array, type is:"
				+ std::to_string(reply->type) + ", domain is:" + domain);
		}
	}

	return false;
}

bool redis_base::auth_db()
{
	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "AUTH redis, domain is:" + domain);
	if (pwd.empty())
	{
		log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "no AUTH, domain is:" + domain);

		return true;
	}
	redisReply *reply = (redisReply *)redisCommand(redis_con, "AUTH %s", pwd.c_str());
	if (0 == exec_command_is_OK(reply))
	{
		log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "AUTH redis success, domain is:" + domain);

		safe_free_reply(&reply);
		return true;
	}
	if (reply && REDIS_REPLY_ERROR == reply->type)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + std::string("redis AUTH failed, err_msg is:") 
			+ reply->str + ", domain is:" + domain);
	}

	safe_free_reply(&reply);
	return false;
}

bool redis_base::ping_pong()
{
	log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "send PING, domain is:" + domain);

	redisReply *reply = (redisReply *)redisCommand(redis_con, "PING");
	if (reply && REDIS_REPLY_STATUS == reply->type && 0 == memcmp(reply->str, "PONG", 4))
	{
		safe_free_reply(&reply);
		log_msg(REDIS_LOG_LEVEL_INFO, THIS_CLASS_FUNC + "recv PONG, domain is:" + domain);
		return true;
	}

	log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "not recv PONG, domain is:" + domain);
	safe_free_reply(&reply);
	return false;
}


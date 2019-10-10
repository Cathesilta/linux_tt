#include "redis_set_op_base.h"

#define THIS_CLASS_FUNC CLASS_FUNC("redis_set_op_base")

redis_set_op_base::redis_set_op_base(redis_log_func_t func /*= nullptr*/, std::string con_host /*= "127.0.0.1"*/, short con_port /*= 6379*/, std::string password /*= ""*/, int con_timeout /*= 0*/) 
	:redis_base(func, con_host, con_port, password, con_timeout)
{

}

redis_set_op_base::~redis_set_op_base()
{

}

bool redis_set_op_base::sadd(const std::string & key, const std::string & member)
{
	//SADD key member
	std::string command = "SADD " + key + " " + member;

	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);
	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SADD key=[" + key + "] member=["
			+ member + "] success");
		//SADD return 1, success
		return true;
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "SADD key=[" + key + "] member=["
			+ member + "] repetition");
	}

	//SADD return 0 or fail
	return false;
}

bool redis_set_op_base::sadd(const std::string& key, const std::vector<std::string> &members)
{
	//SADD key member [member бн]
	std::string command = "SADD " + key + " ";
	for (auto &it : members) {
		command += it + " ";
	}

	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);
	if (1 <= result)
	{

		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SADD key=[" + key + "]");
		for (auto &it : members) {
			log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "member=[" + it + "]");
		}
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "success");
		//SADD return 1, success
		return true;
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SADD key=[" + key + "]");
		for (auto &it : members) {
			log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "member=[" + it + "]");
		}
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "repetition");
	}

	//SADD return 0 or fail
	return false;
}


bool redis_set_op_base::sismember(const std::string& key, const std::string& member)
{
	//SISMEMBER key member
	std::string command = "SISMEMBER " + key + " " + member;

	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);
	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SISMEMBER key=[" + key + "] member=["
			+ member + "] exist");
		//SISMEMBER return 1, is member
		return true;
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "SISMEMBER key=[" + key + "] member=["
			+ member + "] not exist");
	}

	//SISMEMBER return 0 or fail
	return false;
}

std::string redis_set_op_base::spop(const std::string& key)
{
	//SPOP key
	std::string command = "SPOP " + key;

	redisReply* reply = exec_command(command);

	std::string result = get_reply_string(reply);
	if (!result.empty()) {
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SPOP key=[" + key + "] and ["
			+ result + "] is popped out");
		//SPOP poppped out a value
		return result;
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SPOP key=[" + key + "] but popped out nothing");
		//SPOP poppped out nothing
	}

	return "";
}

std::string redis_set_op_base::srandmember(const std::string& key)
{
	//SRANDMEMBER key[count]
	std::string command = "SRANDMEMBER  " + key;

	redisReply* reply = exec_command(command);

	std::string result = get_reply_string(reply);
	if (!result.empty()) {
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SRANDMEMBER key=[" + key + "] and ["
			+ result + "] is returned");
		//SRANDMEMBER returned a value
		return result;
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SRANDMEMBER key=[" + key + "] but returned nothing");
		//SRANDMEMBER returned nil
	}

	return "";
}

std::vector<std::string> redis_set_op_base::srandmember(const std::string& key, long long count)
{
	//SRANDMEMBER key[count]

	return std::vector<std::string>();
}

void redis_set_op_base::srem(const std::string& key, const std::string& val)
{
	//SREM key member [member бн]
	std::string command = "SREM  " + key + " " + val;

	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);

	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SREM key=[" + key + "] val=["
			+ val + "] success");
		//SREM return 1, success
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "SISMEMBER key=[" + key + "] val=["
			+ val + "] not fail, value may not exist");
	}
}

void redis_set_op_base::srem(const std::string& key, const std::vector<std::string>& vals)
{
	//SREM key member [member бн]
	std::string command = "SREM  " + key + " ";
	for (auto &it : vals) {
		command += it;
		command += " ";
	}

	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);

	if (1 <= result)
	{
		//SREM return >=1 , success
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SREM key=[" + key + "] success, removed <" + std::to_string(result)+"> value");
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "SREM key=[" + key + "] failed, there may be no value in the key");
	}
}

void redis_set_op_base::smove(const std::string& source, const std::string& destination, const std::string& member)
{
	//SMOVE source destination member
	std::string command = "SMOVE " + source + " " + destination + " " + member;
	redisReply* reply = exec_command(command);

	long long result = get_reply_integer(reply);
	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + member + " move " + source + "`s member to"
			+ destination + " success");
	}
	else if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + member + " not set " + source + "`s member");
	}
}

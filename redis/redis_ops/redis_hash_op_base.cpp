#include "redis_hash_op_base.h"

#define THIS_CLASS_FUNC CLASS_FUNC("redis_string_op_base")


redis_hash_op_base::redis_hash_op_base(redis_log_func_t func /*= nullptr*/, 
	std::string con_host /*= "127.0.0.1"*/, short con_port /*= 6379*/, 
	std::string password /*= ""*/, int con_timeout /*= 0*/)
	:redis_base(func, con_host, con_port, password, con_timeout)
{

}

redis_hash_op_base::~redis_hash_op_base()
{

}

/*
将哈希表 hash 中域 field 的值设置为 value 。
如果给定的哈希表并不存在， 那么一个新的哈希表将被创建并执行 HSET 操作。
如果域 field 已经存在于哈希表中， 那么它的旧值将被新值 value 覆盖。

返回值
当 HSET 命令在哈希表中新创建 field 域并成功为它设置值时， 命令返回 1 ； 
如果域 field 已经存在于哈希表， 并且 HSET 命令成功使用新值覆盖了它的旧值， 那么命令返回 0 。
*/
void redis_hash_op_base::hset(const std::string& key, const std::string& field, const std::string& value)
{
	//HSET hash field value
	std::string command = "HSET " + key  + " " + field  + " "+ value;
	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "is create, key is:" + key);
	}
	else if(0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "is update, key is:" + key);
	}
}

/*
当且仅当域 field 尚未存在于哈希表的情况下， 将它的值设置为 value 。
如果给定域已经存在于哈希表当中， 那么命令将放弃执行设置操作。
如果哈希表 hash 不存在， 那么一个新的哈希表将被创建并执行 HSETNX 命令。

返回值
HSETNX 命令在设置成功时返回 1 ， 在给定域已经存在而放弃执行设置操作时返回 0 。
*/
bool redis_hash_op_base::hset_nx(const std::string& key, const std::string& field, const std::string& value)
{
	//HSETNX hash field value
	std::string command = "HSETNX " + key + " " + field + " " + value;
	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "is set, key is:" + key);
		return true;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "is exist, key is:" + key);
	return false;
}

/*
返回哈希表中给定域的值。

返回值
HGET 命令在默认情况下返回给定域的值。
如果给定域不存在于哈希表中， 又或者给定的哈希表并不存在， 那么命令返回 nil 。
*/
std::string redis_hash_op_base::get(const std::string& key, const std::string& field)
{

	//HGET hash field
	std::string command = "HGET " + key + " " + field;
	redisReply *reply = exec_command(command);
	std::string result = get_reply_string(reply);
	safe_free_reply(&reply);
	if (false == result.empty())
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC
			+ "key:" + key + ", field:" + field + ", value is:" + result);
	}
	return result;
}

/*
检查给定域 field 是否存在于哈希表 hash 当中。

返回值
HEXISTS 命令在给定域存在时返回 1 ， 在给定域不存在时返回 0 。
*/
bool redis_hash_op_base::hexists(const std::string& key, const std::string& field)
{
	//HEXISTS hash field
	std::string command = "HEXISTS " + key + " " + field;
	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);
	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ", field:" + field + " exist" );
		return true;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ", field:" + field + "  do not exist");
	return false;
}


/*
删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略。

Note
在Redis2.4以下的版本里， HDEL 每次只能删除单个域，如果你需要在一个原子时间内删除多个域，请将命令包含在 MULTI / EXEC 块内。

可用版本：
>= 2.0.0
时间复杂度:
O(N)， N 为要删除的域的数量。
返回值:
被成功移除的域的数量，不包括被忽略的域。
*/
bool redis_hash_op_base::hdel(const std::string& key, const std::string& field)
{
	//HDEL key field [field …]
	std::string command = "HDEL " + key + " " + field;

	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	if (1 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC 
			+ "success, key:" + key + ", field:" + field);
		return true;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC
		+ "failed, key:" + key + ", field:" + field);
	return false;
}

void redis_hash_op_base::hdel(const std::string& key, const std::vector<std::string>& fields)
{
	std::string command = "HDEL " + key ;

	for (auto &it : fields)
	{
		command.append(" ");
		command.append(it);
	}

	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	if (0 == result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "failed, key:" + key);
		return;
	}

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC 
		+ "del num:" + std::to_string(result) + ", key:" + key);
	return;
}

/*
返回哈希表 key 中域的数量。

时间复杂度：
O(1)
返回值：
哈希表中域的数量。
当 key 不存在时，返回 0 。
*/
long long redis_hash_op_base::hlen(const std::string& key)
{
	//HLEN key
	std::string command = "HLEN " + key;

	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC
		+ "field num:" + std::to_string(result) + ", key:" + key);
	return result;
}

/*
返回哈希表 key 中， 与给定域 field 相关联的值的字符串长度（string length）。

如果给定的键或者域不存在， 那么命令返回 0 。
*/
long long redis_hash_op_base::hstrlen(const std::string& key, const std::string& field)
{
	//HSTRLEN key field
	std::string command = "HSTRLEN " + key + " " + field;

	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key 
		+ " ,field:" + field + " ,len is:"+ std::to_string(result));
	return result;
}

/*
为哈希表 key 中的域 field 的值加上增量 increment 。

增量也可以为负数，相当于对给定域进行减法操作。

如果 key 不存在，一个新的哈希表被创建并执行 HINCRBY 命令。

如果域 field 不存在，那么在执行命令前，域的值被初始化为 0 。

对一个储存字符串值的域 field 执行 HINCRBY 命令将造成一个错误。

本操作的值被限制在 64 位(bit)有符号数字表示之内。

可用版本：
>= 2.0.0
时间复杂度：
O(1)
返回值：
执行 HINCRBY 命令之后，哈希表 key 中域 field 的值。
*/
long long redis_hash_op_base::hincrby(const std::string& key, const std::string& field, long long increment)
{
	//HINCRBY key field increment
	std::string command = "HINCRBY " + key + " " + field + " " + std::to_string(increment);

	redisReply* reply = exec_command(command);
	long long result = get_reply_integer(reply);
	safe_free_reply(&reply);

	if (-1 != result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " ,field:" + field
			+ " ,value is:" + std::to_string(result));
	}

	return result;
}

/*
为哈希表 key 中的域 field 加上浮点数增量 increment 。
如果哈希表中没有域 field ，那么 HINCRBYFLOAT 会先将域 field 的值设为 0 ，然后再执行加法操作。
如果键 key 不存在，那么 HINCRBYFLOAT 会先创建一个哈希表，再创建域 field ，最后再执行加法操作。

当以下任意一个条件发生时，返回一个错误：=
域 field 的值不是字符串类型(因为 redis 中的数字和浮点数都以字符串的形式保存，所以它们都属于字符串类型）
域 field 当前的值或给定的增量 increment 不能解释(parse)为双精度浮点数(double precision floating point number)
*/
float redis_hash_op_base::hincrbyfloat(const std::string& key, const std::string& field, float increment)
{

	//HINCRBYFLOAT key field increment
	std::string command = "HINCRBYFLOAT " + key + " " + field + " " + std::to_string(increment);

	redisReply* reply = exec_command(command);
	float result = -1.0;
	try
	{
		result = std::stof(get_reply_string(reply));
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " ,field:" + field
			+ " ,value is:" + std::to_string(result));
	}
	catch (const std::exception& e)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "exception type is:" + typeid(e).name());
	}
	safe_free_reply(&reply);


	return result;
}

/*

同时将多个 field-value (域-值)对设置到哈希表 key 中。
此命令会覆盖哈希表中已存在的域。
如果 key 不存在，一个空哈希表被创建并执行 HMSET 操作。

可用版本：
>= 2.0.0
时间复杂度：
O(N)， N 为 field-value 对的数量。
返回值：
如果命令执行成功，返回 OK 。
当 key 不是哈希表(hash)类型时，返回一个错误。
*/
bool redis_hash_op_base::hmset(const std::string& key, const std::map<std::string, std::string>& f_v)
{
	//HMSET key field value [field value …]
	std::string command = "HMSET " + key;
	for (auto &it : f_v)
	{
		command.append(" ");
		command.append(it.first);
		command.append(" ");
		command.append(it.second);
	}

	redisReply* reply = exec_command(command);
	bool result = exec_command_is_OK(reply);
	safe_free_reply(&reply);

	if (result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "excute success"+ ", key:" + key);
	}

	return result;
}
	

/*
返回哈希表 key 中，一个或多个给定域的值。
如果给定的域不存在于哈希表，那么返回一个 nil 值。
因为不存在的 key 被当作一个空哈希表来处理，所以对一个不存在的 key 进行 HMGET 操作将返回一个只带有 nil 值的表。

可用版本：
>= 2.0.0
时间复杂度：
O(N)， N 为给定域的数量。
返回值：
一个包含多个给定域的关联值的表，表值的排列顺序和给定域参数的请求顺序一样。
*/
std::unordered_map<std::string, std::string > redis_hash_op_base::hmget(const std::string& key, const std::vector<std::string>& fields)
{
	//HMGET key field [field …]
	std::unordered_map<std::string, std::string > infos;
	if (fields.empty())
	{
		return infos;
	}

	std::string command = "HMGET " + key;
	for (auto& it : fields)
	{
		command.append(" ");
		command.append(it);
	}

	redisReply* reply = exec_command(command);

	std::vector<redisReply*> results;
	if (get_reply_array(reply, results))
	{
		size_t size = results.size();
		for (size_t i = 0; i < size; ++i)
		{
			infos.insert(std::make_pair(fields[i], get_reply_string(results[i])));
		}
	}

	safe_free_reply(&reply);

	return infos;
}

/*
返回哈希表 key 中的所有域。

可用版本：
>= 2.0.0
时间复杂度：
O(N)， N 为哈希表的大小。
返回值：
一个包含哈希表中所有域的表。
当 key 不存在时，返回一个空表。
*/
bool redis_hash_op_base::hkeys(const std::string& key, std::vector<std::string>& fields)
{
	//HKEYS key
	std::string command = "HKEYS " + key;
	redisReply* reply = exec_command(command);

	bool result = false;
	std::vector<redisReply*> infos;
	if (get_reply_array(reply, infos))
	{
		size_t size = infos.size();
		for (size_t i = 0; i < size; ++i)
		{
			fields.push_back(get_reply_string(infos[i]));
		}

		result = true;
	}

	safe_free_reply(&reply); 
	return result;
}

/*
HVALS key
返回哈希表 key 中所有域的值。

可用版本：
>= 2.0.0
时间复杂度：
O(N)， N 为哈希表的大小。
返回值：
一个包含哈希表中所有值的表。
当 key 不存在时，返回一个空表。
*/
bool redis_hash_op_base::hvals(const std::string& key, std::vector<std::string>& values)
{
	//HVALS key
	std::string command = "HVALS " + key;
	redisReply* reply = exec_command(command);
	bool result = false;
	std::vector<redisReply*> infos;
	if (get_reply_array(reply, infos))
	{
		size_t size = infos.size();
		for (size_t i = 0; i < size; ++i)
		{
			values.push_back(get_reply_string(infos[i]));
		}

		result = true;
	}

	safe_free_reply(&reply);
	return result;
}

/*
返回哈希表 key 中，所有的域和值。

在返回值里，紧跟每个域名(field name)之后是域的值(value)，所以返回值的长度是哈希表大小的两倍。

可用版本：
>= 2.0.0
时间复杂度：
O(N)， N 为哈希表的大小。
返回值：
以列表形式返回哈希表的域和域的值。
若 key 不存在，返回空列表。
*/
bool redis_hash_op_base::hgetall(const std::string& key, std::unordered_map<std::string, std::string>& f_v)
{
	//HGETALL key
	std::string command = "HGETALL " + key;
	redisReply* reply = exec_command(command);

	bool result = false;
	std::vector<redisReply*> infos;
	if (get_reply_array(reply, infos))
	{
		size_t size = infos.size();
		for (size_t i = 0; i < size; )
		{
			f_v.insert(std::make_pair(get_reply_string(infos[i]), get_reply_string(infos[i + 1])));
			i += 2;
		}

		result = true;
	}

	safe_free_reply(&reply);
	return result;
}
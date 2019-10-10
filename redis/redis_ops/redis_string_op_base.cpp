#include "redis_string_op_base.h"

#define THIS_CLASS_FUNC CLASS_FUNC("redis_string_op_base")

redis_string_op_base::redis_string_op_base(redis_log_func_t func /*= nullptr*/, 
	std::string con_host /*= "127.0.0.1"*/, short con_port /*= 6379*/, 
	std::string password /*= ""*/, int con_timeout /*= 0*/) 
	: redis_base(func, con_host, con_port, password, con_timeout)
{

}

redis_string_op_base::~redis_string_op_base()
{
}


/*
将字符串值 value 关联到 key 。

如果 key 已经持有其他值， SET 就覆写旧值， 无视类型。

当 SET 命令对一个带有生存时间（TTL）的键进行设置之后， 该键原有的 TTL 将被清除。

可选参数
从 Redis 2.6.12 版本开始， SET 命令的行为可以通过一系列参数来修改：

EX seconds ： 将键的过期时间设置为 seconds 秒。 执行 SET key value EX seconds 的效果等同于执行 SETEX key seconds value 。
PX milliseconds ： 将键的过期时间设置为 milliseconds 毫秒。 执行 SET key value PX milliseconds 的效果等同于执行 PSETEX key milliseconds value 。
NX ： 只在键不存在时， 才对键进行设置操作。 执行 SET key value NX 的效果等同于执行 SETNX key value 。
XX ： 只在键已经存在时， 才对键进行设置操作。
Note

因为 SET 命令可以通过参数来实现 SETNX 、 SETEX 以及 PSETEX 命令的效果， 所以 Redis 将来的版本可能会移除并废弃 SETNX 、 SETEX 和 PSETEX 这三个命令。

返回值
在 Redis 2.6.12 版本以前， SET 命令总是返回 OK 。
从 Redis 2.6.12 版本开始， SET 命令只在设置操作成功完成时才返回 OK ； 如果命令使用了 NX 或者 XX 选项， 但是因为条件没达到而造成设置操作未执行， 那么命令将返回空批量回复（NULL Bulk Reply）。
*/
bool redis_string_op_base::set(const std::string& key, const std::string& value, const std::string& extern_command /*= ""*/, long long expire_time/* = 0*/)
{
	//SET key value [EX seconds] [PX milliseconds] [NX|XX]
	bool result = false;
	std::string command = "SET " + key + " " + value ;

	if (0 != extern_command.length())
	{
		command += " " + extern_command;
	}
	if (0 != expire_time)
	{
		command += " " + std::to_string(expire_time);
	}

	redisReply* reply = exec_command(command);
	if ( exec_command_is_OK(reply))
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "is create, key is:" + key);
		result = true;
	}

	safe_free_reply(&reply);

	return result;
}

bool redis_string_op_base::set_ex(const std::string& key, const std::string& value, long long seconds)
{
	return set(key, value, "EX", seconds);
}

bool redis_string_op_base::set_px(const std::string& key, const std::string& value, long long milliseconds)
{
	return set(key, value, "EX", milliseconds);
}

bool redis_string_op_base::set_nx(const std::string& key, const std::string& value)
{
	return set(key, value, "NX");
}

bool redis_string_op_base::set_xx(const std::string& key, const std::string& value)
{
	return set(key, value, "XX");
}

/*
返回与键 key 相关联的字符串值。

返回值
如果键 key 不存在， 那么返回特殊值 nil ； 否则， 返回键 key 的值。
如果键 key 的值并非字符串类型， 那么返回一个错误， 因为 GET 命令只能用于字符串值。
*/
std::string redis_string_op_base::get(const std::string& key)
{
	//GET key
	std::string command = "GET " + key;

	redisReply* reply = exec_command(command);
	std::string value = get_reply_string(reply);

	safe_free_reply(&reply);

	if (0 == value.length())
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " not found");
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",value is:" + value);
	}
	
	return value;
}

/*
将键 key 的值设为 value ， 并返回键 key 在被设置之前的旧值。

返回值
返回给定键 key 的旧值。

如果键 key 没有旧值， 也即是说， 键 key 在被设置之前并不存在， 那么命令返回 nil 。

当键 key 存在但不是字符串类型时， 命令返回一个错误。
*/
std::string redis_string_op_base::getset(const std::string& key, const std::string& new_value)
{
	//GETSET key value
	std::string command = "GETSET " + key + " " + new_value;

	redisReply* reply = exec_command(command);
	std::string old_value = get_reply_string(reply);
	
	safe_free_reply(&reply);
	if (old_value.empty())
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + " not old_value");
	}
	else
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",old_value is:" + old_value);
	}

	return old_value;
}


/*
返回键 key 储存的字符串值的长度。

返回值
STRLEN 命令返回字符串值的长度。

当键 key 不存在时， 命令返回 0 。

当 key 储存的不是字符串值时， 返回一个错误。
*/
long long redis_string_op_base::strlen(const std::string& key)
{
	//STRLEN key
	std::string command = "STRLEN " + key;

	redisReply* reply = exec_command(command);
	long long value_len =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	if (0 == value_len)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",not found");
	}
	else if(0 < value_len)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key + ",value len is:" + std::to_string(value_len));
	}

	return value_len;
}

/*

如果键 key 已经存在并且它的值是一个字符串， APPEND 命令将把 value 追加到键 key 现有值的末尾。

如果 key 不存在， APPEND 就简单地将键 key 的值设为 value ， 就像执行 SET key value 一样。

返回值
追加 value 之后， 键 key 的值的长度。
*/
long long redis_string_op_base::append(const std::string& key, const std::string& value)
{
	//APPEND key value
	std::string command = "APPEND " + key + " " + value;

	redisReply* reply = exec_command(command);
	long long len =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	return len;
}

/*
从偏移量 offset 开始， 用 value 参数覆写(overwrite)键 key 储存的字符串值。

不存在的键 key 当作空白字符串处理。

SETRANGE 命令会确保字符串足够长以便将 value 设置到指定的偏移量上， 如果键 key 原来储存的字符串长度比偏移量小(比如字符串只有 5 个字符长，但你设置的 offset 是 10 )， 那么原字符和偏移量之间的空白将用零字节(zerobytes, "\x00" )进行填充。

因为 Redis 字符串的大小被限制在 512 兆(megabytes)以内， 所以用户能够使用的最大偏移量为 2^29-1(536870911) ， 如果你需要使用比这更大的空间， 请使用多个 key 。

Warning

当生成一个很长的字符串时， Redis 需要分配内存空间， 该操作有时候可能会造成服务器阻塞(block)。 在2010年出产的Macbook Pro上， 设置偏移量为 536870911(512MB 内存分配)将耗费约 300 毫秒， 设置偏移量为 134217728(128MB 内存分配)将耗费约 80 毫秒， 设置偏移量 33554432(32MB 内存分配)将耗费约 30 毫秒， 设置偏移量为 8388608(8MB 内存分配)将耗费约 8 毫秒。

返回值
SETRANGE 命令会返回被修改之后， 字符串值的长度。
*/
long long redis_string_op_base::setrange(const std::string& key, long long offset, const std::string& value)
{
	//SETRANGE key offset value
	std::string command = "SETRANGE ";
	command.append(key);
	command.append(" ");
	command.append(std::to_string(offset));
	command.append(" ");
	command.append(value);

	redisReply* reply = exec_command(command);
	long long len =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	return len;
}

/*
返回键 key 储存的字符串值的指定部分， 字符串的截取范围由 start 和 end 两个偏移量决定 (包括 start 和 end 在内)。
负数偏移量表示从字符串的末尾开始计数， -1 表示最后一个字符， -2 表示倒数第二个字符， 以此类推。
GETRANGE 通过保证子字符串的值域(range)不超过实际字符串的值域来处理超出范围的值域请求。

Note
GETRANGE 命令在 Redis 2.0 之前的版本里面被称为 SUBSTR 命令。

返回值
GETRANGE 命令会返回字符串值的指定部分。
*/
std::string redis_string_op_base::getrange(const std::string& key, long long start, long long end)
{
	//GETRANGE key start end
	std::string command = "GETRANGE ";
	command.append(key);
	command.append(" ");
	command.append(std::to_string(start));
	command.append(" ");
	command.append(std::to_string(end));

	redisReply* reply = exec_command(command);
	std::string value = get_reply_string(reply);
	
	safe_free_reply(&reply);

	return value;
}


/*
为键 key 储存的数字值加上一。
如果键 key 不存在， 那么它的值会先被初始化为 0 ， 然后再执行 INCR 命令。
如果键 key 储存的值不能被解释为数字， 那么 INCR 命令将返回一个错误。
本操作的值限制在 64 位(bit)有符号数字表示之内。

Note
INCR 命令是一个针对字符串的操作。 因为 Redis 并没有专用的整数类型， 所以键 key 储存的值在执行 INCR 命令时会被解释为十进制 64 位有符号整数。

返回值
INCR 命令会返回键 key 在执行加一操作之后的值。
*/
long long redis_string_op_base::incr(const std::string& key)
{
	//INCR key
	std::string command = "INCR " + key;

	redisReply* reply = exec_command(command);
	long long result =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	if (-1 != result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key
			+ ",value is:" + std::to_string(result));
	}
	return result;
}

long long redis_string_op_base::incrby(const std::string& key, long long increment)
{
	//INCRBY key increment
	std::string command = "INCRBY ";
	command.append(key);
	command.append(" ");
	command.append(std::to_string(increment));

	redisReply* reply = exec_command(command);
	long long result =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	if (-1 != result)
	{
		log_msg(REDIS_LOG_LEVEL_DEBUG, THIS_CLASS_FUNC + "key:" + key
			+ ",value is:" + std::to_string(result));
	}
	return result;
}

float redis_string_op_base::incrbyfloat(const std::string& key, float increment)
{
	//INCRBYFLOAT key increment
	std::string command = "INCRBYFLOAT ";
	command.append(key);
	command.append(" ");
	command.append(std::to_string(increment));

	redisReply* reply = exec_command(command);

	float result = -1.0;
	try
	{
		result = std::stof(get_reply_string(reply));
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "key:" + key
			+ ",value is:" + std::to_string(result));
	}
	catch (const std::exception& e)
	{
		log_msg(REDIS_LOG_LEVEL_ERROR, THIS_CLASS_FUNC + "exception type is:" + typeid(e).name());
	}
	
	safe_free_reply(&reply);

	return result;
}

long long redis_string_op_base::decr(const std::string& key)
{
	//DECR key
	std::string command = "DECR " + key;

	redisReply* reply = exec_command(command);
	long long len =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	return len;
}

long long redis_string_op_base::decrby(const std::string& key, long long increment)
{
	//DECRBY key decrement
	std::string command = "DECRBY ";
	command.append(key);
	command.append(" ");
	command.append(std::to_string(increment));

	redisReply* reply = exec_command(command);
	long long len =get_reply_integer(reply);
	
	safe_free_reply(&reply);

	return len;
}

/*
同时为多个键设置值。
如果某个给定键已经存在， 那么 MSET 将使用新值去覆盖旧值， 如果这不是你所希望的效果， 请考虑使用 MSETNX 命令， 这个命令只会在所有给定键都不存在的情况下进行设置。
MSET 是一个原子性(atomic)操作， 所有给定键都会在同一时间内被设置， 不会出现某些键被设置了但是另一些键没有被设置的情况。

返回值
MSET 命令总是返回 OK 。
*/
bool redis_string_op_base::mset(const std::map <std::string,std::string> &kvs)
{
	//MSET key value[key value …]
	if (kvs.empty())
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "kvs is empty:");
		return false;
	}

	std::string command = "MSET ";
	for (auto &it: kvs)
	{
		command.append(it.first);
		command.append(" ");
		command.append(it.second);
		command.append(" ");
	}

	bool result = false;

	redisReply* reply = exec_command(command);
	if ( exec_command_is_OK(reply))
	{
		result = true;
	}

	safe_free_reply(&reply);

	return result;
}

/*
当且仅当所有给定键都不存在时， 为所有给定键设置值。
即使只有一个给定键已经存在， MSETNX 命令也会拒绝执行对所有键的设置操作。
MSETNX 是一个原子性(atomic)操作， 所有给定键要么就全部都被设置， 要么就全部都不设置， 不可能出现第三种状态。

返回值
当所有给定键都设置成功时， 命令返回 1 ； 如果因为某个给定键已经存在而导致设置未能成功执行， 那么命令返回 0 。
*/
bool redis_string_op_base::mset_nx(const std::map<std::string, std::string>& kvs)
{
	//MSETNX key value [key value …]
		//MSET key value[key value …]
	bool result = false;

	if (kvs.empty())
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "kvs is empty:");
		return result;
	}

	std::string command = "MSETNX ";
	for (auto& it : kvs)
	{
		command.append(it.first);
		command.append(" ");
		command.append(it.second);
		command.append(" ");
	}

	redisReply* reply = exec_command(command);
	if (exec_command_is_OK(reply))
	{
		result = true;
	}

	safe_free_reply(&reply);

	return result;
}


/*
返回给定的一个或多个字符串键的值。
如果给定的字符串键里面， 有某个键不存在， 那么这个键的值将以特殊值 nil 表示。

返回值
MGET 命令将返回一个列表， 列表中包含了所有给定键的值。
*/
std::map < std::string, std::string > redis_string_op_base::mget(const std::vector<std::string>& keys)
{
	//MGET key[key …]
	std::map < std::string, std::string > infos;
	if (keys.empty())
	{
		log_msg(REDIS_LOG_LEVEL_WARN, THIS_CLASS_FUNC + "keys is empty:");
		return std::move(infos);
	}

	std::string command = "MGET ";
	for (auto &it : keys)
	{
		command.append(it);
		command.append(" ");
	}

	redisReply* reply = exec_command(command);

	std::vector<redisReply*> results;
	if (get_reply_array(reply,results))
	{
		size_t size = keys.size();
		for (size_t i = 0; i < size; ++i)
		{
			infos.insert(std::make_pair(keys[i],get_reply_string(results[i])));
		}
	}

	safe_free_reply(&reply);

	return std::move(infos);
}

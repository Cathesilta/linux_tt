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
���ַ���ֵ value ������ key ��

��� key �Ѿ���������ֵ�� SET �͸�д��ֵ�� �������͡�

�� SET �����һ����������ʱ�䣨TTL���ļ���������֮�� �ü�ԭ�е� TTL ���������

��ѡ����
�� Redis 2.6.12 �汾��ʼ�� SET �������Ϊ����ͨ��һϵ�в������޸ģ�

EX seconds �� �����Ĺ���ʱ������Ϊ seconds �롣 ִ�� SET key value EX seconds ��Ч����ͬ��ִ�� SETEX key seconds value ��
PX milliseconds �� �����Ĺ���ʱ������Ϊ milliseconds ���롣 ִ�� SET key value PX milliseconds ��Ч����ͬ��ִ�� PSETEX key milliseconds value ��
NX �� ֻ�ڼ�������ʱ�� �ŶԼ��������ò����� ִ�� SET key value NX ��Ч����ͬ��ִ�� SETNX key value ��
XX �� ֻ�ڼ��Ѿ�����ʱ�� �ŶԼ��������ò�����
Note

��Ϊ SET �������ͨ��������ʵ�� SETNX �� SETEX �Լ� PSETEX �����Ч���� ���� Redis �����İ汾���ܻ��Ƴ������� SETNX �� SETEX �� PSETEX ���������

����ֵ
�� Redis 2.6.12 �汾��ǰ�� SET �������Ƿ��� OK ��
�� Redis 2.6.12 �汾��ʼ�� SET ����ֻ�����ò����ɹ����ʱ�ŷ��� OK �� �������ʹ���� NX ���� XX ѡ� ������Ϊ����û�ﵽ��������ò���δִ�У� ��ô������ؿ������ظ���NULL Bulk Reply����
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
������� key ��������ַ���ֵ��

����ֵ
����� key �����ڣ� ��ô��������ֵ nil �� ���� ���ؼ� key ��ֵ��
����� key ��ֵ�����ַ������ͣ� ��ô����һ������ ��Ϊ GET ����ֻ�������ַ���ֵ��
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
���� key ��ֵ��Ϊ value �� �����ؼ� key �ڱ�����֮ǰ�ľ�ֵ��

����ֵ
���ظ����� key �ľ�ֵ��

����� key û�о�ֵ�� Ҳ����˵�� �� key �ڱ�����֮ǰ�������ڣ� ��ô����� nil ��

���� key ���ڵ������ַ�������ʱ�� �����һ������
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
���ؼ� key ������ַ���ֵ�ĳ��ȡ�

����ֵ
STRLEN ������ַ���ֵ�ĳ��ȡ�

���� key ������ʱ�� ����� 0 ��

�� key ����Ĳ����ַ���ֵʱ�� ����һ������
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

����� key �Ѿ����ڲ�������ֵ��һ���ַ����� APPEND ����� value ׷�ӵ��� key ����ֵ��ĩβ��

��� key �����ڣ� APPEND �ͼ򵥵ؽ��� key ��ֵ��Ϊ value �� ����ִ�� SET key value һ����

����ֵ
׷�� value ֮�� �� key ��ֵ�ĳ��ȡ�
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
��ƫ���� offset ��ʼ�� �� value ������д(overwrite)�� key ������ַ���ֵ��

�����ڵļ� key �����հ��ַ�������

SETRANGE �����ȷ���ַ����㹻���Ա㽫 value ���õ�ָ����ƫ�����ϣ� ����� key ԭ��������ַ������ȱ�ƫ����С(�����ַ���ֻ�� 5 ���ַ������������õ� offset �� 10 )�� ��ôԭ�ַ���ƫ����֮��Ŀհ׽������ֽ�(zerobytes, "\x00" )������䡣

��Ϊ Redis �ַ����Ĵ�С�������� 512 ��(megabytes)���ڣ� �����û��ܹ�ʹ�õ����ƫ����Ϊ 2^29-1(536870911) �� �������Ҫʹ�ñ������Ŀռ䣬 ��ʹ�ö�� key ��

Warning

������һ���ܳ����ַ���ʱ�� Redis ��Ҫ�����ڴ�ռ䣬 �ò�����ʱ����ܻ���ɷ���������(block)�� ��2010�������Macbook Pro�ϣ� ����ƫ����Ϊ 536870911(512MB �ڴ����)���ķ�Լ 300 ���룬 ����ƫ����Ϊ 134217728(128MB �ڴ����)���ķ�Լ 80 ���룬 ����ƫ���� 33554432(32MB �ڴ����)���ķ�Լ 30 ���룬 ����ƫ����Ϊ 8388608(8MB �ڴ����)���ķ�Լ 8 ���롣

����ֵ
SETRANGE ����᷵�ر��޸�֮�� �ַ���ֵ�ĳ��ȡ�
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
���ؼ� key ������ַ���ֵ��ָ�����֣� �ַ����Ľ�ȡ��Χ�� start �� end ����ƫ�������� (���� start �� end ����)��
����ƫ������ʾ���ַ�����ĩβ��ʼ������ -1 ��ʾ���һ���ַ��� -2 ��ʾ�����ڶ����ַ��� �Դ����ơ�
GETRANGE ͨ����֤���ַ�����ֵ��(range)������ʵ���ַ�����ֵ������������Χ��ֵ������

Note
GETRANGE ������ Redis 2.0 ֮ǰ�İ汾���汻��Ϊ SUBSTR ���

����ֵ
GETRANGE ����᷵���ַ���ֵ��ָ�����֡�
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
Ϊ�� key ���������ֵ����һ��
����� key �����ڣ� ��ô����ֵ���ȱ���ʼ��Ϊ 0 �� Ȼ����ִ�� INCR ���
����� key �����ֵ���ܱ�����Ϊ���֣� ��ô INCR �������һ������
��������ֵ������ 64 λ(bit)�з������ֱ�ʾ֮�ڡ�

Note
INCR ������һ������ַ����Ĳ����� ��Ϊ Redis ��û��ר�õ��������ͣ� ���Լ� key �����ֵ��ִ�� INCR ����ʱ�ᱻ����Ϊʮ���� 64 λ�з���������

����ֵ
INCR ����᷵�ؼ� key ��ִ�м�һ����֮���ֵ��
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
ͬʱΪ���������ֵ��
���ĳ���������Ѿ����ڣ� ��ô MSET ��ʹ����ֵȥ���Ǿ�ֵ�� ����ⲻ������ϣ����Ч���� �뿼��ʹ�� MSETNX ��� �������ֻ�������и������������ڵ�����½������á�
MSET ��һ��ԭ����(atomic)������ ���и�����������ͬһʱ���ڱ����ã� �������ĳЩ���������˵�����һЩ��û�б����õ������

����ֵ
MSET �������Ƿ��� OK ��
*/
bool redis_string_op_base::mset(const std::map <std::string,std::string> &kvs)
{
	//MSET key value[key value ��]
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
���ҽ������и�������������ʱ�� Ϊ���и���������ֵ��
��ʹֻ��һ���������Ѿ����ڣ� MSETNX ����Ҳ��ܾ�ִ�ж����м������ò�����
MSETNX ��һ��ԭ����(atomic)������ ���и�����Ҫô��ȫ���������ã� Ҫô��ȫ���������ã� �����ܳ��ֵ�����״̬��

����ֵ
�����и����������óɹ�ʱ�� ����� 1 �� �����Ϊĳ���������Ѿ����ڶ���������δ�ܳɹ�ִ�У� ��ô����� 0 ��
*/
bool redis_string_op_base::mset_nx(const std::map<std::string, std::string>& kvs)
{
	//MSETNX key value [key value ��]
		//MSET key value[key value ��]
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
���ظ�����һ�������ַ�������ֵ��
����������ַ��������棬 ��ĳ���������ڣ� ��ô�������ֵ��������ֵ nil ��ʾ��

����ֵ
MGET �������һ���б� �б��а��������и�������ֵ��
*/
std::map < std::string, std::string > redis_string_op_base::mget(const std::vector<std::string>& keys)
{
	//MGET key[key ��]
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

#ifndef redis_type_h__
#define redis_type_h__

#include <functional>
#include <string>

enum redis_log_level :int
{
	REDIS_LOG_LEVEL_DEBUG = 0,
	REDIS_LOG_LEVEL_INFO = 1,
	REDIS_LOG_LEVEL_WARN = 2,
	REDIS_LOG_LEVEL_ERROR = 3
};

#define CLASS_FUNC(class_name) std::string("line:") + std::to_string(__LINE__) + " " + class_name + std::string("::") + std::string(__FUNCTION__) + std::string(" ")


using redis_log_func_t = std::function<void(int, const std::string&)>;

#endif // redis_type_h__

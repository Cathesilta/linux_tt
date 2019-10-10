#ifndef redis_string_test_h__
#define redis_string_test_h__

#include "../redis_ops/redis_string_op_base.h"

#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;

#define TEST_FUNC_COUT 	cout << endl;cout<<"this is "<< __FUNCTION__<<endl;

void string_test(redis_string_op_base & rsob)
{
	TEST_FUNC_COUT

	rsob.set("key","new-value");
	rsob.get("key");

	cout << endl;
	rsob.set_ex("key-with-expire-time","hello",10086);
	rsob.get("key-with-expire-time");
	rsob.ttl("key-with-expire-time");
	rsob.pttl("key");

	cout << endl;
	rsob.set_px("key-with-pexpire-time", "moto",123321);
	rsob.get("key-with-pexpire-time");
	rsob.pttl("key-with-pexpire-time");

	cout << endl;
	rsob.set_nx("not-exists-key","value");
	rsob.get("not-exists-key");
	rsob.set_nx("not-exists-key", "new-value");
	rsob.get("not-exists-key");

	cout << endl;
	rsob.set_xx("exists-key", "value");
	rsob.get("exists-key");
	rsob.set("exists-key", "value");
	rsob.set_xx("exists-key", "new-value");
	rsob.get("exists-key");

	cout << endl;
	rsob.getset("exists-key2", "new-new-value");
	rsob.getset("exists-key","new-new-value");

	cout << endl;
	rsob.set("mykey", "new-value");
	rsob.strlen("mykey");
	rsob.strlen("nonexisting");

	cout << endl;
	rsob.exists("myphone");
	long long  app_len = rsob.append("myphone", "nokia");
	cout << app_len << endl;
	app_len = rsob.append("myphone", "-1110");
	cout << app_len << endl;

	cout << endl;
	
	rsob.set("greeting", "hello world");
	rsob.exec_command("set greeting \"hello world\"");
	rsob.setrange("greeting", 6,"redis");
	rsob.get("greeting");

	rsob.setrange("empty_string", 5, "redis");
	rsob.get("empty_string");

	cout << endl;
	rsob.set(R"("getrange_test")", R"("hello,my friend")");
	string result;
	result = rsob.getrange("getrange_test", 0, 4);
	cout << result << endl;
	result = rsob.getrange("getrange_test", -1, -5);
	cout << result << endl;
	result = rsob.getrange("getrange_test", -3, -1);
	cout << result << endl;
	result = rsob.getrange("getrange_test", 0, -1);
	cout << result << endl;
	result = rsob.getrange("getrange_test", 0, 1008611);
	cout << result << endl;

	cout << endl;
	rsob.set("page_view", to_string(20));
	rsob.incr("page_view");
	rsob.set("page_view2", "233test");
	rsob.incr("page_view2");
	rsob.set("page_view3", "test233");
	rsob.incr("page_view3");

	cout << endl;
	rsob.set("rank", to_string(50));
	rsob.incrby("rank", 50);

	cout << endl;
	rsob.incrbyfloat("rank", 2.5);
	rsob.incrbyfloat("page_view2", 2.5);

	cout << endl;
	std::map <std::string, std::string> values;
	values["string_1"] = "string_1_str";
	values["string_2"] = "string_2_str";
	values["string_3"] = "string_3_str";
	rsob.mset(values);
	std::vector<std::string> keys;
	keys.push_back("string_1");
	keys.push_back("string_2");
	keys.push_back("string_3");
	std::map<std::string,std::string>  k_vs= rsob.mget(keys);
	cout << k_vs["string_1"] << endl;
	cout << k_vs["string_2"] << endl;
	cout << k_vs["string_3"] << endl;
}

#endif // redis_string_test_h__
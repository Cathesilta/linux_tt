#ifndef redis_hash_test_h__
#define redis_hash_test_h__

#include "../redis_ops/redis_hash_op_base.h"

#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;

#define TEST_FUNC_COUT 	cout << endl;cout<<"this is "<< __FUNCTION__<<endl;

void hash_test(redis_hash_op_base &rhob)
{
	TEST_FUNC_COUT
	rhob.hset("website", "google", "www.g.cn");
	rhob.get("website", "google");
	rhob.hset("website", "google", "www.google.com");
	rhob.get("website", "google");
	rhob.get("website", "baidu");

	cout << endl;
	rhob.hexists("website", "baidu");
	rhob.hexists("website", "google");
	rhob.hset_nx("website", "google", "www.g.cn");
	rhob.hset_nx("website", "baidu", "www.baidu.cn");

	cout << endl;
	rhob.hdel("website", "baidu");
	rhob.hdel("website", "tengxun");
	
	cout << endl;
	rhob.hlen("website");
	rhob.hstrlen("website", "google");

	cout << endl;
	rhob.hexists("counter", "page_view");
	rhob.hincrby("counter", "page_view", 200);
	rhob.get("counter", "page_view");
	rhob.hincrby("counter", "page_view", -50);
	rhob.get("counter", "page_view");

	cout << endl;
	rhob.hset("myhash", "string", "hello,world");
	rhob.get("myhash", "string");
	rhob.hincrby("myhash", "string", 200);
	rhob.get("myhash", "string");

	cout << endl;
	rhob.hincrbyfloat("mykey","field", 10.50);
	rhob.exec_command("HSET mykey field 5.0e3");
	rhob.exec_command("HINCRBYFLOAT mykey field 2.0e2");
	rhob.get("mykey", "field");

	cout << endl;
	rhob.hexists("price", "milk");
	rhob.hincrbyfloat("price", "milk", 3.5);
	unordered_map<string, string> info1;
	rhob.hgetall("price", info1);

	cout << endl;
	rhob.hincrbyfloat("price", "coffee", 3.5);
	rhob.hgetall("price", info1);

	cout << endl;
	vector<string> fields;
	rhob.hkeys("price", fields);
	vector<string> fields2;
	rhob.hkeys("ttt", fields2);
	vector<string> values;
	rhob.hvals("price", values);
	vector<string> values2;
	rhob.hvals("ttt", values2);

	cout << endl;
	map<string, string> info2;
	info2["yahoo"] = "www.yahoo.com";
	info2["tengxun"] = "www.tengxun.com";
	rhob.hmset("website", info2);
	vector<string> fields_tt = { "yahoo" ,"tengxun" ,"ali"};
	unordered_map<string, string> info3 = rhob.hmget("website", fields_tt);
	rhob.exec_command("HMSET website field");

}

#endif // redis_hash_test_h__

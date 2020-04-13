#pragma once


#include <string_view>
#include <string>
#include <fstream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"

class JsonParser
{
public:
	JsonParser();
	~JsonParser();

	bool load_json(std::string_view&& file_name);

	bool get_int32(std::string_view&& tag, int32_t& out_data);
	bool get_uint32(std::string_view&& tag, uint32_t& out_data);
	bool get_int64(std::string_view&&  tag, int64_t& out_data);
	bool get_uint64(std::string_view&& tag, uint64_t& out_data);
	bool get_boolean(std::string_view&& tag, bool& out_data);
	bool get_single(std::string_view&& tag, float& out_data);
	bool get_double(std::string_view&& tag, double& out_data);
	bool get_string(std::string_view&& tag, std::string& out_data);


private:
	rapidjson::Document buffer;
};

//#include <iostream>
//#include "JsonParser.h"
//
//
//int main()
//{
//	JsonParser parser;
//	if (false == parser.load_json("config.json"))
//	{
//		std::cout << "zz" << std::endl;;
//
//		return 1;
//	}
//
//	bool t = true;
//	float tt = 0;
//	std::string ttt;
//	parser.get_boolean("f", t);
//	parser.get_single("pi", tt);
//	parser.get_string("hello", ttt);
//
//	std::cout << t << std::endl;;
//	std::cout << tt << std::endl;;
//	std::cout << ttt << std::endl;;
//}
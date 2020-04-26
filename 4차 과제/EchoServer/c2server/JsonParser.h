#pragma once


#include <string_view>
#include <string>
#include <fstream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"


namespace c2::util
{

	class JsonParser
	{
	public:
		JsonParser();
		~JsonParser();

		bool load_json(std::wstring_view&& file_name);

		bool get_uint16(std::wstring_view&& tag, uint16_t& out_data);
		bool get_int32(std::wstring_view&& tag, int32_t& out_data);
		bool get_uint32(std::wstring_view&& tag, uint32_t& out_data);
		bool get_int64(std::wstring_view&& tag, int64_t& out_data);
		bool get_uint64(std::wstring_view&& tag, uint64_t& out_data);
		bool get_boolean(std::wstring_view&& tag, bool& out_data);
		bool get_single(std::wstring_view&& tag, float& out_data);
		bool get_double(std::wstring_view&& tag, double& out_data);
		bool get_wstring(std::wstring_view&& tag, std::wstring& out_data);
		bool get_raw_wstring(std::wstring_view&& tag, wchar_t* dest, size_t raw_str_length);



	private:
		rapidjson::GenericDocument<rapidjson::UTF16LE<>> document;
	};
}
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
//	std::wstring ttt;
//	parser.get_boolean("f", t);
//	parser.get_single("pi", tt);
//	parser.get_string("hello", ttt);
//
//	std::cout << t << std::endl;;
//	std::cout << tt << std::endl;;
//	std::cout << ttt << std::endl;;
//}
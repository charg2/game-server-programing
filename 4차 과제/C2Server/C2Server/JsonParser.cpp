
#include <fstream>
#include <iterator>
#include <iostream>
#include <codecvt>
#include "JsonParser.h"

JsonParser::JsonParser()
{
}

JsonParser::~JsonParser()
{}

bool JsonParser::load_json(std::wstring_view&& file_name)
{
	std::ifstream in(file_name.data(), std::ios::binary);
	if (!in)
	{
		return false;
	}
	
	std::wstring temp{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	
	buffer.Parse(temp.c_str(), temp.size());
	if (true == buffer.HasParseError())
	{
		return false;
	}

	return true;
}

bool JsonParser::get_int32(std::wstring_view&& tag, int32_t& out_data)
{
	out_data = buffer[tag.data()].GetInt();

	return true;
}

bool JsonParser::get_uint32(std::wstring_view&& tag,  uint32_t& out_data)
{
	out_data = buffer[tag.data()].GetUint();

	return true;

}

bool JsonParser::get_int64(std::wstring_view&& tag, int64_t& out_data)
{
	out_data = buffer[tag.data()].GetInt64();
	return true;

}

bool JsonParser::get_uint64(std::wstring_view&& tag, uint64_t& out_data)
{
	out_data = buffer[tag.data()].GetUint64();
	return true;

}

bool JsonParser::get_boolean(std::wstring_view&& tag, bool& out_data)
{
	out_data = buffer[tag.data()].GetBool();
	return true;

}

bool JsonParser::get_single(std::wstring_view&& tag, float& out_data)
{
	out_data = buffer[tag.data()].GetFloat();
	return true;
}

bool JsonParser::get_double(std::wstring_view&& tag, double& out_data)
{
	out_data = buffer[tag.data()].GetDouble();
	return true;

}


bool JsonParser::get_wstring(std::wstring_view&& tag, std::wstring& out_data)
{
	out_data = buffer[tag.data()].GetString();

	return true;
}


//#include <type_traits>
//#include <string>
//#include <iostream>
////#include "OuterServer.h"
//
//#include "JsonParser.h"

//
//int main()
//{
//	JsonParser parser;
//	if (false == parser.load_json(L"config.json"))
//	{
//		std::cout << "zz" << std::endl;;
//
//		return 1;
//	}
//
//	bool t = true;
//	float tt = 0;
//	std::wstring ttt;
//	parser.get_boolean(L"enable_nagle_opt", t);
//	parser.get_single(L"server_version", tt);
//	parser.get_wstring(L"server_name", ttt);
//
//	std::cout << t << std::endl;;
//	std::cout << tt << std::endl;;
//	std::wcout << ttt << std::endl;;
//
//	parser.get_wstring(L"server_ip", ttt);
//	std::wcout << ttt << std::endl;;
//}


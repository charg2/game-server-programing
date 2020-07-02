
#include <fstream>
#include <iterator>
#include <iostream>
#include <codecvt>
#include "JsonParser.h"



namespace c2::util
{

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

		document.Parse(temp.c_str(), temp.size());
		if (true == document.HasParseError())
		{
			return false;
		}

		return true;
	}

	bool JsonParser::get_uint16(std::wstring_view&& tag, uint16_t& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetInt();

		return true;
	}

	bool JsonParser::get_int32(std::wstring_view&& tag, int32_t& out_data)
	{
		if ( false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetInt();

		return true;
	}

	bool JsonParser::get_uint32(std::wstring_view&& tag, uint32_t& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetUint();

		return true;
	}

	bool JsonParser::get_int64(std::wstring_view&& tag, int64_t& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}


		out_data = document[tag.data()].GetInt64();
		return true;
	}

	bool JsonParser::get_uint64(std::wstring_view&& tag, uint64_t& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetUint64();
		return true;
	}

	bool JsonParser::get_boolean(std::wstring_view&& tag, bool& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetBool();
		return true;
	}

	bool JsonParser::get_single(std::wstring_view&& tag, float& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetFloat();
		return true;
	}

	bool JsonParser::get_double(std::wstring_view&& tag, double& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}


		out_data = document[tag.data()].GetDouble();
		return true;
	}


	bool JsonParser::get_wstring(std::wstring_view&& tag, std::wstring& out_data)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		out_data = document[tag.data()].GetString();

		return true;
	}

	bool JsonParser::get_raw_wstring(std::wstring_view&& tag, wchar_t* dest, size_t raw_str_length)
	{
		if (false == document.HasMember(tag.data()))
		{
			return false;
		}

		std::wstring wstr = document[tag.data()].GetString();
		
		size_t length = wstr.length();
		if (length - 1 > raw_str_length) // 공백까지 생각해ㅣ서
		{
			return false;
		}

		wmemcpy(dest, wstr.c_str(), length);
		dest[length] = NULL;



		return true;
	}

	bool JsonParser::get_map(std::wstring_view&& tag, char* map, size_t width, size_t height)
	{
		const auto& arr = document[tag.data()];

		for (rapidjson::SizeType i = 0; i < arr.Size(); i++) // Uses SizeType instead of size_t
		{
			const wchar_t* str = arr[i].GetString();
			for (int n{}; n < arr[i].GetStringLength(); ++n)
			{
				map[i * width + n] = (char)str[n] - '0';
				//printf("%d", map[i * width + n]);
			}
			//printf("\n");
		}
		return true;
	}

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


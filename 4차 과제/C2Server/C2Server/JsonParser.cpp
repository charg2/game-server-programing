
#include <fstream>
#include <string>
#include <iterator>
#include <iostream>

#include "JsonParser.h"

JsonParser::JsonParser()
{
}

JsonParser::~JsonParser()
{}

bool JsonParser::load_json(std::string_view&& file_name)
{
	std::ifstream in(file_name.data(), std::ios::binary);
	if (!in)
	{
		return false;
	}
	
	std::string temp{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	
	buffer.Parse(temp.c_str(), temp.size());
	if (true == buffer.HasParseError())
	{
		return false;
	}

	return true;
}

bool JsonParser::get_int32(std::string_view&& tag, int32_t& out_data)
{
	out_data = buffer[tag.data()].GetInt();

	return true;
}

bool JsonParser::get_uint32(std::string_view&& tag,  uint32_t& out_data)
{
	out_data = buffer[tag.data()].GetUint();

	return true;

}

bool JsonParser::get_int64(std::string_view&& tag, int64_t& out_data)
{
	out_data = buffer[tag.data()].GetInt64();
	return true;

}

bool JsonParser::get_uint64(std::string_view&& tag, uint64_t& out_data)
{
	out_data = buffer[tag.data()].GetUint64();
	return true;

}

bool JsonParser::get_boolean(std::string_view&& tag, bool& out_data)
{
	out_data = buffer[tag.data()].GetBool();
	return true;

}

bool JsonParser::get_single(std::string_view&& tag, float& out_data)
{
	out_data = buffer[tag.data()].GetFloat();
	return true;
}

bool JsonParser::get_double(std::string_view&& tag, double& out_data)
{
	out_data = buffer[tag.data()].GetDouble();
	return true;

}


bool JsonParser::get_string(std::string_view&& tag, std::string& out_data)
{
	out_data = buffer[tag.data()].GetString();

	return true;
}






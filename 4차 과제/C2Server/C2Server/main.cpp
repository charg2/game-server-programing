#include <type_traits>
#include <string>
#include <iostream>
//#include "OuterServer.h"

#include "JsonParser.h"

int main()
{
	JsonParser parser;
	if (false == parser.load_json(L"config.json"))
	{
		std::cout << "zz" << std::endl;;

		return 1;
	}

	bool t = true;
	float tt = 0;
	std::wstring ttt;
	parser.get_boolean(L"enable_nagle_opt", t);
	parser.get_single(L"server_version", tt);
	parser.get_wstring(L"server_name", ttt);

	std::cout << t << std::endl;;
	std::cout << tt << std::endl;;
	std::wcout << ttt << std::endl;;

	parser.get_wstring(L"server_ip", ttt);
	std::wcout << ttt << std::endl;;
}	
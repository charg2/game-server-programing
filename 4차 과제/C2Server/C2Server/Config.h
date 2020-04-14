#pragma once

#include "JsonParser.h"

struct ConfigFormat
{

};

class DynamicConfig
{
	DynamicConfig();
	~DynamicConfig();
	
	void load();


private:
	ConfigFormat* data;
};


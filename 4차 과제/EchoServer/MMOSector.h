#pragma once

#include <vector>

class MMOActor;
class MMOSector
{
public:
	MMOSector();
	~MMOSector();
	
private:
	std::vector<MMOActor*> actors;
};



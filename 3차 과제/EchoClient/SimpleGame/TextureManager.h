#pragma once

#include <unordered_map>

// -1 ½ÇÆĞ
class Renderer;
class TextureManager
{
public:
	TextureManager(Renderer* renderer);

	int  load_texture(char* file_path, char* file_name);
	int  get_texture(char*); 
private:
	Renderer*					   renderer;
	std::unordered_map<char*, int> texture_map;
};

extern TextureManager* gTextureManager;
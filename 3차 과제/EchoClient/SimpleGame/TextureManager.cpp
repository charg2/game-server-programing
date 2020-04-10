#include "stdafx.h"
#include "Renderer.h"
#include "TextureManager.h"

TextureManager* gTextureManager { nullptr };

TextureManager::TextureManager(Renderer* renderer) : renderer{ renderer }
{
}

int TextureManager::load_texture(char* file_path, char* file_name)
{
	this->renderer->GenPngTexture(file_path);
	return 0;
}

int TextureManager::get_texture(char* file_name)
{
	return 0;
}

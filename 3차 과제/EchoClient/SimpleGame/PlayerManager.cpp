//#include "PlayerManager.h"
//
//std::unique_ptr<PlayerManager> g_player_manager{ std::make_unique<PlayerManager>() };
//
//PlayerManager::PlayerManager()
//{}
//
//PlayerManager::~PlayerManager()
//{}
//
//OtherPlayer* PlayerManager::get_other_player(uint64_t session_id)
//{
//	return nullptr;
//}
//
//bool PlayerManager::insert_other_player(uint64_t session_id, OtherPlayer* other)
//{
//	return this->insert({ session_id, other }).second;
//}
//
//bool PlayerManager::remove_other_player(uint64_t session_id)
//{
//	size_t result = this->erase(session_id);
//	
//	return true;
//}
//
//



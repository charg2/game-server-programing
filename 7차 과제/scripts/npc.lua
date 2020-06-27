my_id = 9999;

target_id = -1;

-- 배치 좌표 이름 타입 레벨등 정보 스크리트로 저장.
monster_peace_fixed = 0;
monster_peace_roaming = 1;
monster_combat_fixed = 2;
monster_combat_roaming = 3;





function prepare_npc_script(id)
	my_id = id;
	counter = 0;
	target_id = -1;
end

function load_mob_data(mob_type)
	local x, y, hp, dmg, exp, roaming;

	if monster_peace_fixed == mob_type
	then
		x = math.random(10, 20);
		y = 2;
		exp = 10;
		hp = 55;
		dmg = 20;
		roaming = false;

	elseif monster_peace_roaming == mob_type
	then
		x = 2;
		y = 2;
		exp = 10;
		hp = 55;
		dmg = 20;
		roaming = true;

	elseif monster_combat_fixed  == mob_type
	then
		x = 2;
		y = 2;
		exp = 10;
		hp = 55;
		dmg = 20;
		roaming = false;

	elseif monster_combat_roaming == mob_type
	then
		x = 2;
		y = 2;
		exp = 10;
		hp = 55;
		dmg = 20;
		roaming = true;
	end

	return x, y, hp, dmg, exp, roaming;
end

-- 플레이어가 깨우면. 
function event_palayer_move(actor_id, x, y)
	if server_get_npc_x(my_id) == x
	then
		if server_get_npc_y(my_id) == y
		then
			target_id = actor_id;
			server_send_chatting(target_id, my_id, "hello"); 
			server_npc_move_to_anywhere(my_id);
			counter = counter + 1;
		end
	end
end

function event_palayer_move2(actor_id, x, y)
	if server_get_npc_x(my_id) == x
	then
		if server_get_npc_y(my_id) == y
		then
			target_id = actor_id;
			server_send_chatting(target_id, my_id, "hello"); 
			server_npc_move_to_anywhere(my_id);
			counter = counter + 1;
		end
	end
end


function update_script()
	if counter >= 3 
	then
		server_send_chatting(target_id, my_id, "bye"); 
		counter = 0;
	else
		server_npc_move_to_anywhere(my_id);
		counter = counter + 1;
	end
end



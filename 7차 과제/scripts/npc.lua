my_id = 9999;
target_id = -1;

-- ��ġ ��ǥ �̸� Ÿ�� ������ ���� ��ũ��Ʈ�� ����.
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
	if monster_peace_fixed == mob_type
	then
		name = string.format("LOG_F%d", my_id);

		x = math.random(0, 400 -1);
		y = math.random(0, 400 -1);

		level = 1;
		hp = 100;
		dmg = 20;

	elseif monster_peace_roaming == mob_type
	then
		name = string.format("LOG_R%d", my_id);

		x = math.random(0, 400 -1);
		y = math.random(400 -1, 800 -1);
		
		level = 2;
		hp = 100;
		dmg = 20;

	elseif monster_combat_fixed == mob_type
	then
		name = string.format("OGRE_F%d", my_id);

		x = math.random(400 -1, 800 -1);
		y = math.random(0, 400-1);
		
		level = 3;
		hp = 100;
		dmg = 20;

	elseif monster_combat_roaming == mob_type
	then
		name = string.format("OGRE_R%d", my_id);

		x = math.random(400 -1, 800 - 1);
		y = math.random(400 -1, 800 - 1);
		
		level = 5;
		hp = 55;
		dmg = 20;
	end

	exp = level * 7;
	return name, x, y, hp, dmg, level, exp;
end

-- �÷��̾ �����. 
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



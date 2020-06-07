my_id = 9999;
counter = 0;
target_id = -1;

function prepare_npc_script(id)
	my_id = id;
	counter = 0;
	target_id = -1;
end


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






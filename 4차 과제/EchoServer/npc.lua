my_id = 9999;
cur_state = 0;

function set_npc_id( id )
	my_id = id;
end

function event_player_move(npc_id)
	if npc_get_x(npc_id) == npc_get_x(myid) 
	then
	   if npc_get_y(npc_id) == npc_get_y(myid) 
	   then
	      API_send_message(myid, player, "HELLO");
		end
	end   
end






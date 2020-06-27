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
	local x, y, hp, dmg, level, exp;

	if monster_peace_fixed == mob_type
	then
		x = math.random(0, 400);
		y = math.random(0, 400);

		level = 1;
		hp = 50;
		dmg = 20;

	elseif monster_peace_roaming == mob_type
	then
		x = math.random(0, 400);
		y = math.random(400, 800);

		level = 2;
		hp = 100;
		dmg = 20;

	elseif monster_combat_fixed  == mob_type
	then
		x = math.random(400, 800);
		y = math.random(0, 400);

		level = 3;
		hp = 150;
		dmg = 20;

	elseif monster_combat_roaming == mob_type
	then
		x = math.random(400, 800);
		y = math.random(400, 800);

		level = 4;
		hp = 200;
		dmg = 20;

	end

	exp = level * 5;

	return x, y, hp, dmg, level, exp;
end
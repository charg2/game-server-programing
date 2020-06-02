#include "core/pre_compile.h"
#include "MMOActor.h"
#include "MMOZone.h"
#include "MMOServer.h"

MMOZone::MMOZone()
{
	// position to idx table
	int idx_y = 0, idx_x = 0;
	for (int y = 0; y < c2::constant::MAP_HEIGHT; y += 1)
	{
		if (y % c2::constant::SECTOR_HEIGHT == 0 && y != 0)
			idx_y += 1;

		postion_height_mapling_table[y] = idx_y;
	}
	
	for (int x = 0; x < c2::constant::MAP_WIDTH; x += 1)
	{
		if (x % c2::constant::SECTOR_WIDTH == 0 && x != 0)
			idx_x += 1;

		postion_width_mapling_table[x] = idx_x;
	}
	

////////////
	for (int y = 0; y < c2::constant::MAP_NAX_HEIGHT_INDEX; y += 1)
	{
		for (int x = 0; x < c2::constant::MAP_NAX_WIDTH_INDEX; x += 1)
		{
			sectors[y][x].index_y = y;
			sectors[y][x].index_x = x;
			sectors[y][x].zone = this;
			sectors[y][x].calculate_serctor_idx(); // ���� �ø��ٸ� ������ ����;
		}
	}

	printf("init map... Ok\n ");
	printf("MMOZone Ready... Ok\n ");
}

MMOZone::~MMOZone()
{
}



// �α��ν� ���.
void MMOZone::enter_actor(MMOActor* actor)
{
	MMOSector* current_sector = get_sector(actor);			// view_list �ܾ����.
	//if ( nullptr == actor->current_sector ) // ó�� ����.
		//current_sector = get_sector(actor);			// view_list �ܾ����.
	//else
		//current_sector = actor->current_sector;

	AcquireSRWLockExclusive(&current_sector->lock);							//�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
	actor->current_sector = current_sector;
	current_sector->actors.emplace(actor->get_id(), actor);
	ReleaseSRWLockExclusive(&current_sector->lock); //�� view_list �� �����ϱ� ���� ���ؼ� ���� ��� 
}

void MMOZone::leave_actor(MMOActor* actor)
{
	//this->actors.erase(actor->get_id());
}

void MMOZone::move_sector(MMOActor* actor, MMOSector* start, MMOSector* end)
{

}




MMOSector* MMOZone::get_sector(MMOActor* actor)
{
	return &this->sectors[postion_width_mapling_table[actor->y]][postion_width_mapling_table[actor->x]];
}

MMOSector* MMOZone::get_sector(int32_t y, int32_t x)
{
	return &this->sectors[postion_width_mapling_table[y]][postion_width_mapling_table[x]];
}




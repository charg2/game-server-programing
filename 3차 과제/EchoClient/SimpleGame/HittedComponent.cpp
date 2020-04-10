#include "stdafx.h"
#include "HittedComponent.h"

HitComponent::HitComponent(GameObject* owner, float recovery_time /* =0.2f */) 
	: hit_recovery_time{ recovery_time }, hit_timer{ 0.f }, owner{ owner }, is_hitted{ false }
{
}

HitComponent::~HitComponent()
{
}

bool HitComponent::can_hitted()
{
	return is_hitted == false;
}

void HitComponent::update(float elapsed_time)
{
	if (true == is_hitted)
	{
		this->hit_timer += elapsed_time;
		if ( this->hit_recovery_time < this->hit_timer )
		{
			is_hitted = false;
			this->hit_timer = 0.f;
		}
	}
}

void HitComponent::hit()
{
	if ( this->can_hitted() )
		this->is_hitted = true;
}

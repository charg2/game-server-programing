#pragma once

// ��... ���� �� �س�����.. 
class StateMachine
{
public:
	StateMachine();
	virtual ~StateMachine();

	virtual void render() = 0;
	virtual void update(float elpased_time) = 0;
	virtual void update_after_collision(float elpased_time) = 0;
	virtual void exit() = 0;
};


#pragma once
#include "Job.h"

class Character;

class AnimJob : public Job
{
public:
	AnimJob(Character* pChar);
	~AnimJob();

	void DoIt() override;
private:
	Character* mCharacter;
};
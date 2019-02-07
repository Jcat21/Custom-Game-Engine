// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "game.h"
#include "Physics.h"
#include <assert.h>

int main()
{
	Game game;
	
	bool isOk = true;

	isOk &= Physics::UnitTest();
	assert(isOk);
	if (isOk){
		return 0;
	}
	else {
		return 1;
	}
}


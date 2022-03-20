#pragma once
#include "fmod.h"

class AudioListener
{
public:
	void setPosition(const FMOD_VECTOR newPosition);
	void setVelocity(const FMOD_VECTOR newVelocity);

	void update();
private:
	FMOD_VECTOR position;
	FMOD_VECTOR velocity;
};


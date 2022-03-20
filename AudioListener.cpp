#include "AudioListener.h"
#include "Utils/AudioEngine.h"
void AudioListener::setPosition(const FMOD_VECTOR newPosition)
{
	position = newPosition;
}

void AudioListener::setVelocity(const FMOD_VECTOR newVelocity)
{
	velocity = newVelocity;
}

void AudioListener::update()
{
	FMOD_VECTOR up = FMOD_VECTOR{ 0,0,1 };
	FMOD_VECTOR forward = FMOD_VECTOR{ 1,0,0 };

	AudioEngine::instance()->getSystem()->set3DListenerAttributes(1.0f, &position,&velocity, &forward, &up);
}

#include "AudioEngine.h"
#include "fmod_errors.h"

int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD is not ok " << FMOD_ErrorString(result) <<std::endl;
		return 1;
#ifdef _DEBUG
		__debugbreak();
#endif


	}

	return 0;
}

void AudioEngine::init()
{
	ErrorCheck(FMOD::System_Create(&pSystem));
	ErrorCheck(pSystem->init(32, FMOD_INIT_NORMAL, nullptr));
}

void AudioEngine::update()
{
	ErrorCheck(pSystem->update());
}

void AudioEngine::shutdown()
{
	ErrorCheck(pSystem->close());
	ErrorCheck(pSystem->release());
}

void AudioEngine::loadSound(const std::string& soundName, const std::string& fileName, bool b3d, bool bLooping, bool bStream)
{
	//check if loaded
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{
		//sound already loaded
		return;
	}

	FMOD_MODE mode = FMOD_DEFAULT;
	mode != (b3d) ? FMOD_3D : FMOD_2D;
	mode != (bLooping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode != (bStream) ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;


	FMOD::Sound* loadedSound;
	ErrorCheck(pSystem->createSound(fileName.c_str(), mode, nullptr, &loadedSound));
	
	if (loadedSound != nullptr)
	{
		sounds[soundName] = loadedSound;

	}

}

void AudioEngine::unloadSound(const std::string& soundName)
{
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{

		ErrorCheck(foundElement->second->release());
		sounds.erase(foundElement);
	}
}

void AudioEngine::playSoundByName(const std::string& soundName)
{

	pSystem->playSound(sounds[soundName], nullptr, false, nullptr);
}

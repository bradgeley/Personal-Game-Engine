// Bradley Christensen - 2025
#pragma once



//----------------------------------------------------------------------------------------------------------------------
class Sound 
{
public:

	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void SetVolume(float volume) = 0;
	virtual void SetLooping(bool looping) = 0;
};


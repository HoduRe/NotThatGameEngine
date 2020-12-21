#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;

class Channels {

public:

	Channels(std::string _name);
	~Channels();

public:

	std::string name;
	std::map<float, float3> positionKeys;
	std::map<float, Quat> rotationKeys;
	std::map<float, float3> scaleKeys;

};

class AnimationData {

public:

	AnimationData(std::string _name, float _duration, float _ticks, int _channels, bool _playing = false);
	~AnimationData();

public:

	std::string name;
	float duration;
	float ticksPerSecond;
	int channelsAmount;
	std::vector<Channels> channels;
	bool playing;

};

class Animation : public Component {

public:

	Animation(long long int _id, GameObject* _gameObject);
	~Animation();
	void PlayAnimation();

public:

	std::vector<AnimationData> animationVec;

};

#endif
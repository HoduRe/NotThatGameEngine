#include "Animation.h"

Channels::Channels(std::string _name) : name(_name), positionKeys(), rotationKeys(), scaleKeys() {}


Channels::~Channels() {}


AnimationData::AnimationData(std::string _name, float _duration, float _ticks, int _channels) :
	name(_name), duration(_duration), ticksPerSecond(_ticks), channelsAmount(_channels), channels() {}


AnimationData::~AnimationData() {}


Animation::Animation(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::ANIMATION), animationVec() {}


Animation::~Animation() {}




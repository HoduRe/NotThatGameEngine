#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/src/MathGeoLib.h"

class GameObject;
class Mesh;

struct Channels {

	~Channels();

	std::map<float, float3> positionKeys;
	std::map<float, Quat> rotationKeys;
	std::map<float, float3> scaleKeys;

};

class AnimationData {	// This is the resources, the "animations"

public:

	AnimationData(std::string _name, float _duration, float _ticks, bool _loop = false, bool _playing = false);
	~AnimationData();

public:

	std::string name;
	float duration;
	float ticksPerSecond;
	float realDuration;
	std::map<std::string, Channels> channels;
	bool playing;
	bool loop;

};

class Animation : public Component {	// This is the component, the "animator"

public:

	Animation(long long int _id, GameObject* _gameObject);
	~Animation();
	void PlayAnimation();

private:

	void UpdateGameObjectsTransform(const AnimationData* data);
	void UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, float currentFrame);
	void UpdateMesh(GameObject* gameObject);
	float3 GetUpdatedChannelPosition(const Channels* channel, const float currentFrame, float3 objectPosition) const;
	Quat GetUpdatedChannelRotation(const Channels* channel, const float currentFrame, Quat objectRotation) const;
	float3 GetUpdatedChannelScale(const Channels* channel, const float currentFrame, float3 objectScale) const;
	void AnimateMesh(Mesh* mesh);

public:

	std::vector<AnimationData> animationVec;

private:

	int currentAnimationIndex;
	float time;

};

#endif
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

struct AnimationEvent {

	AnimationEvent(int _animationId, float _animationKeyFrame, int _eventId, bool _onlyOnce = false, bool _eventTriggered = false);

	int animationId;
	float animationKeyFrame;
	int eventId;
	bool onlyOnce;
	bool eventTriggered;

};

class AnimationData {	// This is the resources, the "animations"

public:

	AnimationData(std::string _name, float _duration, float _ticks, bool _loop = false, bool _playing = false);
	~AnimationData();

public:

	std::string name;
	std::map<std::string, Channels> channels;

	float duration;
	float ticksPerSecond;
	float realDuration;
	float time;

	bool playing;
	bool loop;
	bool started;

	bool hasEnded;

};

class Animation : public Component {	// This is the component, the "animator"

public:

	Animation(long long int _id, GameObject* _gameObject);
	~Animation();
	void PlayAnimation();
	void StartAnimation(int index, bool loop = false);

private:

	void UpdateGameObjectsTransform(const AnimationData* data);
	void UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, float currentFrame, float previousFrame);
	void UpdateMesh(GameObject* gameObject);
	float3 GetUpdatedChannelPosition(const Channels* channel, const float currentFrame, float3 objectPosition) const;
	Quat GetUpdatedChannelRotation(const Channels* channel, const float currentFrame, Quat objectRotation) const;
	float3 GetUpdatedChannelScale(const Channels* channel, const float currentFrame, float3 objectScale) const;
	void AnimateMesh(Mesh* mesh);
	void ManageAnimationEvents();

public:

	std::vector<AnimationData> animationVec;	// Resource part
	std::vector<AnimationEvent> animationEventVec;	// Component part

private:

	int currentAnimationIndex;
	int previousAnimationIndex;

};

#endif
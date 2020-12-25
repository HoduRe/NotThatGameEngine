#include "Animation.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

Channels::~Channels() {

	positionKeys.clear();
	rotationKeys.clear();
	scaleKeys.clear();

}


AnimationData::AnimationData(std::string _name, float _duration, float _ticks, int _channels, bool _playing) :
	name(_name), duration(_duration), ticksPerSecond(_ticks), channelsAmount(_channels), channels(), playing(_playing) {}


AnimationData::~AnimationData() {}


Animation::Animation(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::ANIMATION), animationVec(),
time(0.0f), currentAnimationIndex(-1) {}


Animation::~Animation() {}


void Animation::PlayAnimation() {

	for (uint i = 0; i < animationVec.size(); i++) {

		if (animationVec[i].playing) {

			currentAnimationIndex = i;
			time += 0.01;
			UpdateGameObjectsTransform(&animationVec[i]);

		}

	}

}


void Animation::UpdateGameObjectsTransform(const AnimationData* data) {

	int currentFrame = time * data->ticksPerSecond;
	UpdateGameObjectsTransformRecursively(owner, data, currentFrame);

}


void Animation::UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, int currentFrame) {

	for (int i = 0; i < gameObject->childs.size(); i++) {

		if (data->channels.count(gameObject->name) == 1) {

			Channels* channel = (Channels*)&data->channels.find(gameObject->name)->second;

			gameObject->childs[i]->transform->SetPosition(GetUpdatedChannelPosition(channel, currentFrame));
			gameObject->childs[i]->transform->SetRotation(GetUpdatedChannelRotation(channel, currentFrame));
			gameObject->childs[i]->transform->SetScale(GetUpdatedChannelScale(channel, currentFrame));

		}

		UpdateGameObjectsTransformRecursively(gameObject->childs[i], data, currentFrame);

	}

}


float3 Animation::GetUpdatedChannelPosition(const Channels* channel, const int currentFrame) const {

	float3 newPosition(0, 0, 0);

	if (channel->positionKeys.size() > 0) {

		std::map<float, float3>::const_iterator it = channel->positionKeys.lower_bound(currentFrame);
		if (it != channel->positionKeys.begin()) { it--; }
		newPosition = it->second;

	}

	return newPosition;

}


Quat Animation::GetUpdatedChannelRotation(const Channels* channel, const int currentFrame) const {

	Quat newRotation(0, 0, 0, 0);

	if (channel->rotationKeys.size() > 0) {

		std::map<float, Quat>::const_iterator it = channel->rotationKeys.lower_bound(currentFrame);
		if (it != channel->rotationKeys.begin()) { it--; }
		newRotation = it->second;

	}

	return newRotation;

}


float3 Animation::GetUpdatedChannelScale(const Channels* channel, const int currentFrame) const {

	float3 newScale(2, 2, 2);

	if (channel->scaleKeys.size() > 0) {

		std::map<float, float3>::const_iterator it = channel->scaleKeys.lower_bound(currentFrame);
		if (it != channel->scaleKeys.begin()) { it--; }
		newScale = it->second;

	}

	return newScale;

}


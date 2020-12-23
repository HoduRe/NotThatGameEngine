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
time(0.0f) {}


Animation::~Animation() {}


void Animation::PlayAnimation() {

	for (uint i = 0; i < animationVec.size(); i++) {

		if (animationVec[i].playing) {

			UpdateBones(&animationVec[i]);

		}

	}

}


void Animation::UpdateBones(const AnimationData* data) {

	int currentFrame = time * data->ticksPerSecond;

	for (int i = 0; i < owner->childs.size(); i++) { UpdateBonesRecursively(owner->childs[i], data, currentFrame); }

}


void Animation::UpdateBonesRecursively(GameObject* gameObject, const AnimationData* data, int currentFrame) {

	for (int i = 0; i < gameObject->childs.size(); i++) {
	
		if (gameObject->childs[i]->mesh != nullptr) {

			Mesh* mesh = gameObject->childs[i]->mesh;

			for (int j = 0; j < mesh->boneNamesVec.size(); j++) {

				Channels* channel = (Channels*)&data->channels.find(mesh->boneNamesVec[i])->second;

				gameObject->childs[i]->transform->SetPosition(GetUpdatedChannelPosition(channel, currentFrame));
				gameObject->childs[i]->transform->SetRotation(GetUpdatedChannelRotation(channel, currentFrame));
				gameObject->childs[i]->transform->SetScale(GetUpdatedChannelScale(channel, currentFrame));

			}

		}

		UpdateBonesRecursively(gameObject->childs[i], data, currentFrame);
	
	}

}


float3 Animation::GetUpdatedChannelPosition(const Channels* channel, const int currentFrame) const {

	float3 newPosition(0, 0, 0);

	if (channel->positionKeys.size() > 0) {

		newPosition = channel->positionKeys.lower_bound(currentFrame)->second;

	}

	return newPosition;

}


Quat Animation::GetUpdatedChannelRotation(const Channels* channel, const int currentFrame) const {

	Quat newRotation(0, 0, 0, 0);

	if (channel->rotationKeys.size() > 0) {

		newRotation = channel->rotationKeys.lower_bound(currentFrame)->second;

	}

	return newRotation;

}


float3 Animation::GetUpdatedChannelScale(const Channels* channel, const int currentFrame) const {

	float3 newScale(2, 2, 2);

	if (channel->scaleKeys.size() > 0) {

		newScale = channel->scaleKeys.lower_bound(currentFrame)->second;

	}

	return newScale;

}


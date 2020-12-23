#include "Animation.h"
#include "GameObject.h"
#include "Mesh.h"

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

	// Get position, rotation and scale from channels to update each bone
		// All channels are in the root
		// Bones are each on each mesh

	// Set the bone's new position, rotation and scale

	for (int i = 0; i < gameObject->childs.size(); i++) {
	
		if (gameObject->childs[i]->mesh != nullptr) {

			Mesh* mesh = gameObject->childs[i]->mesh;

			for (int j = 0; j < mesh->boneNamesVec.size(); j++) {

				Channels* channel = (Channels*)&data->channels.find(mesh->boneNamesVec[i])->second;

			}

		}

		UpdateBonesRecursively(gameObject->childs[i], data, currentFrame);
	
	}

}




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
			UpdateMesh(owner);

		}

	}

}


void Animation::UpdateGameObjectsTransform(const AnimationData* data) {

	int currentFrame = time * data->ticksPerSecond;
	UpdateGameObjectsTransformRecursively(owner, data, currentFrame);

}


void Animation::UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, int currentFrame) {


	if (data->channels.count(gameObject->name) == 1) {

		Channels* channel = (Channels*)&data->channels.find(gameObject->name)->second;

		gameObject->transform->SetPosition(GetUpdatedChannelPosition(channel, currentFrame));
		gameObject->transform->SetRotation(GetUpdatedChannelRotation(channel, currentFrame));
		gameObject->transform->SetScale(GetUpdatedChannelScale(channel, currentFrame));

	}

	for (int i = 0; i < gameObject->childs.size(); i++) { UpdateGameObjectsTransformRecursively(gameObject->childs[i], data, currentFrame); }

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

	float3 newScale(0, 0, 0);

	if (channel->scaleKeys.size() > 0) {

		std::map<float, float3>::const_iterator it = channel->scaleKeys.lower_bound(currentFrame);
		if (it != channel->scaleKeys.begin()) { it--; }
		newScale = it->second;

	}

	return newScale;

}


void GetGameObjects(GameObject* gameObject, std::map<std::string, GameObject*>* map) {

	map->insert(std::pair<std::string, GameObject*>(gameObject->name, gameObject));

	for (int i = 0; i < gameObject->childs.size(); i++) { GetGameObjects(gameObject->childs[i], map); }

}


void Animation::UpdateMesh(GameObject* gameObject) {

	for (uint i = 0; i < gameObject->childs.size(); i++) {

		if (gameObject->childs[i]->mesh != nullptr) { AnimateMesh(gameObject->childs[i]->mesh); }

		UpdateMesh(gameObject->childs[i]);

	}

}


void Animation::AnimateMesh(Mesh* mesh) {

	mesh->verticesANIMATION.clear();
	mesh->normalsANIMATION.clear();

	std::map<int, float4x4> skinningMatrixMap;
	std::map<std::string, GameObject*> gameObjectMap;

	GetGameObjects(owner, &gameObjectMap);

	for (int i = 0; i < mesh->boneNamesVec.size(); i++) {

		GameObject* bone = gameObjectMap.find(mesh->boneNamesVec[i])->second;
		skinningMatrixMap[i] = mesh->owner->worldTransform.Inverted() * bone->worldTransform * mesh->boneOffsetMatrixVec[i];

	}

	int size = mesh->vertices.size() / 3;
	for (uint vertexIndex = 0; vertexIndex < size; vertexIndex++) {

		for (uint vertexBones = 0; vertexBones < 4; vertexBones++) {

			int boneID = mesh->boneIDs[vertexIndex * 4 + vertexBones];
			float boneWeight = mesh->boneWeights[vertexIndex * 4 + vertexBones];

			if (boneID != -1) {

				float3 newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->vertices[vertexIndex * 3]));

				mesh->verticesANIMATION.push_back(mesh->vertices[vertexIndex * 3] += newDeviation.x * boneWeight);
				mesh->verticesANIMATION.push_back(mesh->vertices[vertexIndex * 3 + 1] += newDeviation.y * boneWeight);
				mesh->verticesANIMATION.push_back(mesh->vertices[vertexIndex * 3 + 2] += newDeviation.z * boneWeight);

				if (mesh->normals.size() > 0) {

					float3 newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->normals[vertexIndex * 3]));

					mesh->normalsANIMATION.push_back(mesh->normals[vertexIndex * 3] += newDeviation.x * boneWeight);
					mesh->normalsANIMATION.push_back(mesh->normals[vertexIndex * 3 + 1] += newDeviation.y * boneWeight);
					mesh->normalsANIMATION.push_back(mesh->normals[vertexIndex * 3 + 2] += newDeviation.z * boneWeight);

				}

			}

		}

	}

	if (mesh->vertexIdANIMATION != 0) { glDeleteBuffers(1, &mesh->vertexIdANIMATION); }
	if (mesh->normalsIdANIMATION != 0) { glDeleteBuffers(1, &mesh->normalsIdANIMATION); }
	OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexIdANIMATION, mesh->verticesANIMATION.size(), mesh->verticesANIMATION.data());
	if (mesh->normalsANIMATION.size() > 0) { OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsIdANIMATION, mesh->normalsANIMATION.size(), mesh->normalsANIMATION.data()); }

}




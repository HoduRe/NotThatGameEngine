#include "Animation.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

Channels::~Channels() {

	positionKeys.clear();
	rotationKeys.clear();
	scaleKeys.clear();

}


AnimationData::AnimationData(std::string _name, float _duration, float _ticks, bool _playing) :
	name(_name), duration(_duration), ticksPerSecond(_ticks), channels(), playing(_playing) {}


AnimationData::~AnimationData() {}


Animation::Animation(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::ANIMATION), animationVec(),
time(0.0f), currentAnimationIndex(-1) {}


Animation::~Animation() {}


void Animation::PlayAnimation() {

	bool animation = false;

	for (uint i = 0; i < animationVec.size(); i++) {

		if (animationVec[i].playing) {

			currentAnimationIndex = i;
			time += 1/60;
			UpdateGameObjectsTransform(&animationVec[i]);
			UpdateMesh(owner);
			animation = true;

		}

	}

	if (animation == false) { currentAnimationIndex = -1; }

}


void Animation::UpdateGameObjectsTransform(const AnimationData* data) {

	float currentFrame = time * data->ticksPerSecond;
	UpdateGameObjectsTransformRecursively(owner, data, currentFrame);

}


void Animation::UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, float currentFrame) {


	if (data->channels.count(gameObject->name) == 1) {

		Channels* channel = (Channels*)&data->channels.find(gameObject->name)->second;

		gameObject->transform->SetPosition(GetUpdatedChannelPosition(channel, currentFrame, gameObject->transform->GetPosition()));
		gameObject->transform->SetRotation(GetUpdatedChannelRotation(channel, currentFrame, gameObject->transform->GetEulerQuat()));
		gameObject->transform->SetScale(GetUpdatedChannelScale(channel, currentFrame, gameObject->transform->GetScale()));

	}

	for (int i = 0; i < gameObject->childs.size(); i++) { UpdateGameObjectsTransformRecursively(gameObject->childs[i], data, currentFrame); }

}


float3 Animation::GetUpdatedChannelPosition(const Channels* channel, const float currentFrame, float3 objectPosition) const {

	float3 newPosition(objectPosition);

	if (channel->positionKeys.size() > 0) {

		std::map<float, float3>::const_iterator it = channel->positionKeys.lower_bound(currentFrame);
		if (it != channel->positionKeys.begin()) { it--; }
		newPosition += it->second;

	}

	return newPosition;

}


Quat Animation::GetUpdatedChannelRotation(const Channels* channel, const float currentFrame, Quat objectRotation) const {

	Quat newRotation(objectRotation);

	if (channel->rotationKeys.size() > 0) {

		std::map<float, Quat>::const_iterator it = channel->rotationKeys.lower_bound(currentFrame);
		if (it != channel->rotationKeys.begin()) { it--; }
		newRotation = newRotation * it->second;

	}

	return newRotation;

}


float3 Animation::GetUpdatedChannelScale(const Channels* channel, const float currentFrame, float3 objectScale) const {

	float3 newScale(objectScale);

	if (channel->scaleKeys.size() > 0) {

		std::map<float, float3>::const_iterator it = channel->scaleKeys.lower_bound(currentFrame);
		if (it != channel->scaleKeys.begin()) { it--; }
		newScale.x *= it->second.x;	// This keeps scale proportions, but I'm not sure if the way ASSIMP things it should be read is that if scale = 0.5 it means it decreases its scale by half, or if it
		newScale.y *= it->second.y;	// decreases to 0.5. If it's the first this is fine, but if not, we would have to keep a modelScale and an animationScale to keep user modified scale and animation scale
		newScale.z *= it->second.z;	// at the same time. However, since scale is probably not going to be altered, I leave it like this and I trust it will be good enough ):v

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

	if (mesh->boneNamesVec.size() != 0) {

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

					float3 newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->vertices[vertexIndex * 3], mesh->vertices[vertexIndex * 3 + 1], mesh->vertices[vertexIndex * 3 + 2]));

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

}




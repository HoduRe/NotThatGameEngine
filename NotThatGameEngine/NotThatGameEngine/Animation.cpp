#include "Animation.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"

Channels::~Channels() {

	positionKeys.clear();
	rotationKeys.clear();
	scaleKeys.clear();

}


AnimationData::AnimationData(std::string _name, float _duration, float _ticks, bool _loop, bool _playing) :
	name(_name), duration(_duration), ticksPerSecond(_ticks), channels(), loop(_loop), playing(_playing), time(0.0f), started(false)
{
	realDuration = duration / ticksPerSecond;
}


AnimationData::~AnimationData() {}


Animation::Animation(long long int _id, GameObject* _gameObject) : Component(_id, _gameObject, COMPONENT_TYPE::ANIMATION), animationVec(),
currentAnimationIndex(-1), previousAnimationIndex(-1) {}


Animation::~Animation() {}


void Animation::PlayAnimation() {

	bool animation = false;

	for (uint i = 0; i < animationVec.size(); i++) {

		if (animationVec[i].playing && animationVec[i].time <= animationVec[i].realDuration) {

			if (animationVec[i].started == false) {

				animationVec[i].started = true;

				if (currentAnimationIndex == -1) { currentAnimationIndex = i; }
				else if (previousAnimationIndex == -1) {

					previousAnimationIndex = currentAnimationIndex;
					currentAnimationIndex = i;

				}
				else {

					animationVec[i].started = false;
					animationVec[i].playing = false;

				}

			}

			else { animationVec[i].time += 1.0f / 60.0f; }

			animation = true;

		}

		else if (animationVec[i].playing == false || animationVec[i].time > animationVec[i].realDuration) {

			animationVec[i].time = 0;
			if (animationVec[i].loop == false) {

				animationVec[i].playing = false;
				animationVec[i].started = false;
				if (currentAnimationIndex == i) { currentAnimationIndex = -1; }
				if (previousAnimationIndex == i) { previousAnimationIndex = -1; }

			}
			else if (animationVec[i].playing) { animation = true; }

		}

	}

	if (animation == false) {

		currentAnimationIndex = -1;
		previousAnimationIndex = -1;

	}

	else {

		if (currentAnimationIndex != -1) {
			UpdateGameObjectsTransform(&animationVec[currentAnimationIndex]);
			UpdateMesh(owner);
		}

	}

}


void Animation::UpdateGameObjectsTransform(const AnimationData* data) {

	float currentFrame = data->time * data->ticksPerSecond;
	float previousFrame = -1.0f;
	if (previousAnimationIndex != -1) { previousFrame = animationVec[previousAnimationIndex].time * animationVec[previousAnimationIndex].ticksPerSecond; }
	UpdateGameObjectsTransformRecursively(owner, data, currentFrame, previousFrame);

}


void Animation::UpdateGameObjectsTransformRecursively(GameObject* gameObject, const AnimationData* data, float currentFrame, float previousFrame) {

	if (data->channels.count(gameObject->name) == 1) {

		Channels* channel = (Channels*)&data->channels.find(gameObject->name)->second;

		float3 position = GetUpdatedChannelPosition(channel, currentFrame, gameObject->transform->GetPosition());
		Quat rotation = GetUpdatedChannelRotation(channel, currentFrame, gameObject->transform->GetEulerQuat());
		float3 scale = GetUpdatedChannelScale(channel, currentFrame, gameObject->transform->GetScale());

		if (previousFrame != -1.0f) {

			Channels* previousChannel = (Channels*)&animationVec[previousAnimationIndex].channels.find(gameObject->name)->second;
			float blendWeight = (animationVec[previousAnimationIndex].duration - animationVec[previousAnimationIndex].time) / animationVec[previousAnimationIndex].duration;

			position = float3::Lerp(GetUpdatedChannelPosition(previousChannel, previousFrame, gameObject->transform->GetPosition()), position, blendWeight);
			rotation = Quat::Slerp(GetUpdatedChannelRotation(previousChannel, previousFrame, gameObject->transform->GetEulerQuat()), rotation, blendWeight);
			scale = float3::Lerp(GetUpdatedChannelScale(previousChannel, previousFrame, gameObject->transform->GetScale()), scale, blendWeight);

		}

		gameObject->transform->SetPosition(position);
		gameObject->transform->SetRotation(rotation);
		gameObject->transform->SetScale(scale);

	}

	for (int i = 0; i < gameObject->childs.size(); i++) { UpdateGameObjectsTransformRecursively(gameObject->childs[i], data, currentFrame, previousFrame); }

}


float3 Animation::GetUpdatedChannelPosition(const Channels* channel, const float currentFrame, float3 objectPosition) const {

	float3 newPosition(objectPosition);

	if (channel->positionKeys.size() > 0 && channel->positionKeys.begin()->first != -1) {

		std::map<float, float3>::const_iterator prevIt = channel->positionKeys.lower_bound(currentFrame);
		if (prevIt != channel->positionKeys.begin()) { prevIt--; }

		std::map<float, float3>::const_iterator nextIt = channel->positionKeys.upper_bound(currentFrame);
		if (nextIt == channel->positionKeys.end()) { nextIt--; }

		if (prevIt == nextIt) { newPosition = prevIt->second; }	// Without this, we end up with a weight calculation of x / 0, so... Yeah, important
		else { newPosition = prevIt->second.Lerp(nextIt->second, ((currentFrame - prevIt->first) / (nextIt->first - prevIt->first))); }

	}

	return newPosition;

}


Quat Animation::GetUpdatedChannelRotation(const Channels* channel, const float currentFrame, Quat objectRotation) const {

	Quat newRotation(objectRotation);

	if (channel->rotationKeys.size() > 0 && channel->rotationKeys.begin()->first != -1) {

		std::map<float, Quat>::const_iterator prevIt = channel->rotationKeys.lower_bound(currentFrame);
		if (prevIt != channel->rotationKeys.begin()) { prevIt--; }

		std::map<float, Quat>::const_iterator nextIt = channel->rotationKeys.upper_bound(currentFrame);
		if (nextIt == channel->rotationKeys.end()) { nextIt--; }

		if (prevIt == nextIt) { newRotation = prevIt->second; }	// Without this, we end up with a weight calculation of x / 0, so... Yeah, important
		else { newRotation = prevIt->second.Slerp(nextIt->second, ((currentFrame - prevIt->first) / (nextIt->first - prevIt->first))); }

	}

	return newRotation;

}


float3 Animation::GetUpdatedChannelScale(const Channels* channel, const float currentFrame, float3 objectScale) const {

	float3 newScale(objectScale);

	if (channel->scaleKeys.size() > 0 && channel->scaleKeys.begin()->first != -1) {

		std::map<float, float3>::const_iterator prevIt = channel->scaleKeys.lower_bound(currentFrame);
		if (prevIt != channel->scaleKeys.begin()) { prevIt--; }

		std::map<float, float3>::const_iterator nextIt = channel->scaleKeys.upper_bound(currentFrame);
		if (nextIt == channel->scaleKeys.end()) { nextIt--; }

		if (prevIt == nextIt) { newScale = prevIt->second; }	// Without this, we end up with a weight calculation of x / 0, so... Yeah, important
		else { newScale = prevIt->second.Lerp(nextIt->second, ((currentFrame - prevIt->first) / (nextIt->first - prevIt->first))); }

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

		mesh->isAnimatedWithBones = true;

		RELEASE_ARRAY(mesh->verticesANIMATION);
		RELEASE_ARRAY(mesh->normalsANIMATION);

		std::map<int, float4x4> skinningMatrixMap;
		std::map<std::string, GameObject*> gameObjectMap;

		GetGameObjects(owner, &gameObjectMap);

		for (int i = 0; i < mesh->boneNamesVec.size(); i++) {

			GameObject* boneGameObject = gameObjectMap.find(mesh->boneNamesVec[i])->second;
			skinningMatrixMap[i] = (mesh->owner->transform->transform.Inverted() * (owner->transform->transform.Inverted() * boneGameObject->worldTransform)) * mesh->boneOffsetMatrixVec[i];

		}

		mesh->verticesANIMATION = new float[mesh->vertexSize];
		mesh->normalsANIMATION = new float[mesh->vertexSize];
		int size = mesh->vertexSize / 3;
		for (uint vertexIndex = 0; vertexIndex < size; vertexIndex++) {

			for (uint vertexBones = 0; vertexBones < 4; vertexBones++) {

				int boneID = mesh->boneIDs[(vertexIndex * 4) + vertexBones];
				float boneWeight = mesh->boneWeights[(vertexIndex * 4) + vertexBones];

				if (boneID != -1) {

					int vectorVertexIndex = vertexIndex * 3;

					float3 newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->vertices[vectorVertexIndex], mesh->vertices[vectorVertexIndex + 1], mesh->vertices[vectorVertexIndex + 2]));

					if (vertexBones == 0) {

						mesh->verticesANIMATION[vectorVertexIndex] = (newDeviation.x * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 1] = (newDeviation.y * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 2] = (newDeviation.z * boneWeight);

					}

					else {

						mesh->verticesANIMATION[vectorVertexIndex] += (newDeviation.x * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 1] += (newDeviation.y * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 2] += (newDeviation.z * boneWeight);

					}


					if (mesh->normalsSize > 0) {

						newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->normals[vectorVertexIndex]));

						if (vertexBones == 0) {

							mesh->normalsANIMATION[vectorVertexIndex] = (newDeviation.x * boneWeight);
							mesh->normalsANIMATION[vectorVertexIndex + 1] = (newDeviation.y * boneWeight);
							mesh->normalsANIMATION[vectorVertexIndex + 2] = (newDeviation.z * boneWeight);

						}

						else {

							mesh->normalsANIMATION[vectorVertexIndex] += (newDeviation.x * boneWeight);
							mesh->normalsANIMATION[vectorVertexIndex + 1] += (newDeviation.y * boneWeight);
							mesh->normalsANIMATION[vectorVertexIndex + 2] += (newDeviation.z * boneWeight);

						}

					}

				}

			}

		}

		if (mesh->vertexIdANIMATION != 0) { glDeleteBuffers(1, &mesh->vertexIdANIMATION); }
		if (mesh->normalsIdANIMATION != 0) { glDeleteBuffers(1, &mesh->normalsIdANIMATION); }
		OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexIdANIMATION, mesh->vertexSize, mesh->verticesANIMATION);
		if (mesh->normalsSize > 0) { OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsIdANIMATION, mesh->normalsSize, mesh->normalsANIMATION); }

	}

}




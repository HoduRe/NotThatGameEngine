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

		if (animationVec[i].playing && time <= animationVec[i].duration) {

			currentAnimationIndex = i;
			time += 1.0f / 60.0f;
			UpdateGameObjectsTransform(&animationVec[i]);
			UpdateMesh(owner);
			animation = true;

		}

		else if (animationVec[i].playing == false || time > animationVec[i].duration) {

			time = 0;
			animationVec[i].playing = false;

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

	if (channel->positionKeys.size() > 0 && channel->positionKeys.begin()->first != -1) {

		std::map<float, float3>::const_iterator it = channel->positionKeys.lower_bound(currentFrame);
		if (it != channel->positionKeys.begin()) { it--; }
		newPosition = it->second;

	}

	return newPosition;

}


Quat Animation::GetUpdatedChannelRotation(const Channels* channel, const float currentFrame, Quat objectRotation) const {

	Quat newRotation(objectRotation);

	if (channel->rotationKeys.size() > 0 && channel->rotationKeys.begin()->first != -1) {

		std::map<float, Quat>::const_iterator it = channel->rotationKeys.lower_bound(currentFrame);
		if (it != channel->rotationKeys.begin()) { it--; }
		newRotation = it->second;

	}

	return newRotation;

}


float3 Animation::GetUpdatedChannelScale(const Channels* channel, const float currentFrame, float3 objectScale) const {

	float3 newScale(objectScale);

	if (channel->scaleKeys.size() > 0 && channel->scaleKeys.begin()->first != -1) {

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

	if (mesh->boneNamesVec.size() != 0) {

		mesh->isAnimatedWithBones = true;

		mesh->verticesANIMATION.clear();
		mesh->normalsANIMATION.clear();

		std::map<int, float4x4> skinningMatrixMap;
		std::map<std::string, GameObject*> gameObjectMap;

		GetGameObjects(owner, &gameObjectMap);

		for (int i = 0; i < mesh->boneNamesVec.size(); i++) {

			GameObject* boneGameObject = gameObjectMap.find(mesh->boneNamesVec[i])->second;
			skinningMatrixMap[i] = (owner->transform->transform.Inverted() * boneGameObject->worldTransform) * mesh->boneOffsetMatrixVec[i];

		}

		int size = mesh->vertices.size() / 3;
		for (uint vertexIndex = 0; vertexIndex < size; vertexIndex++) {

			for (uint vertexBones = 0; vertexBones < 4; vertexBones++) {

				int boneID = mesh->boneIDs[(vertexIndex * 4) + vertexBones];
				float boneWeight = mesh->boneWeights[(vertexIndex * 4) + vertexBones];

				if (boneID != -1) {

					int vectorVertexIndex = vertexIndex * 3;

					float3 newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->vertices[vectorVertexIndex], mesh->vertices[vectorVertexIndex + 1], mesh->vertices[vectorVertexIndex + 2]));
					float verticesX = mesh->vertices[vectorVertexIndex];
					float verticesY = mesh->vertices[vectorVertexIndex + 1];
					float verticesZ = mesh->vertices[vectorVertexIndex + 2];

					if (vertexBones == 0) {

						mesh->verticesANIMATION.push_back(verticesX + (newDeviation.x * boneWeight));
						mesh->verticesANIMATION.push_back(verticesY + (newDeviation.y * boneWeight));
						mesh->verticesANIMATION.push_back(verticesZ + (newDeviation.z * boneWeight));

					}

					else {

						mesh->verticesANIMATION[vectorVertexIndex] += (newDeviation.x * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 1] += (newDeviation.y * boneWeight);
						mesh->verticesANIMATION[vectorVertexIndex + 2] += (newDeviation.z * boneWeight);

					}


					if (mesh->normals.size() > 0) {

						newDeviation = skinningMatrixMap[boneID].TransformPos(float3(mesh->normals[vectorVertexIndex]));
						float normalsX = mesh->normals[vectorVertexIndex];
						float normalsY = mesh->normals[vectorVertexIndex + 1];
						float normalsZ = mesh->normals[vectorVertexIndex + 2];

						if (vertexBones == 0) {

							mesh->normalsANIMATION.push_back(normalsX + (newDeviation.x * boneWeight));
							mesh->normalsANIMATION.push_back(normalsY + (newDeviation.y * boneWeight));
							mesh->normalsANIMATION.push_back(normalsZ + (newDeviation.z * boneWeight));

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
		OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexIdANIMATION, mesh->verticesANIMATION.size(), mesh->verticesANIMATION.data());
		if (mesh->normalsANIMATION.size() > 0) { OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsIdANIMATION, mesh->normalsANIMATION.size(), mesh->normalsANIMATION.data()); }

	}

}




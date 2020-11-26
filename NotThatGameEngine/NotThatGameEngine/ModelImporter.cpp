#include "OpenGLFuncionality.h"
#include "Application.h"
#include "ModelImporter.h"
#include "EditorScene.h"
#include "ManagerExternal.h"
#include "GameObject.h"
#include "ManagerResource.h"
#include "Save.h"
#include "Load.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"

GameObject* ModelImporter::LoadNewModel(Application* App, const char* path, const char* buffer, uint size, GameObject* parent, bool enabled) {

	std::string filePath = path;
	ResourceEnum type;
	std::string originalName;

	App->externalManager->SplitFilePath(path, nullptr, &originalName);
	GameObject* newObject = new GameObject(App->idGenerator.Int(), originalName, parent, enabled);
	
	if (!App->resourceManager->IsLoadedInLibrary(&filePath, &type)) {

		if (buffer == nullptr && size == 0) { size = App->externalManager->Load(path, (char**)&buffer); }
		if (ModelImporter::LoadNewModelComponents(App, buffer, size, newObject, path)) {

			DataSaving::SaveModel(App, newObject, originalName);
			RecursiveChildCallToChangeID(App, newObject);

		}

		else {
			delete newObject;
			newObject = nullptr;
		}

		RELEASE_ARRAY(buffer);

	}

	else {

		char* bufferAux = nullptr;
		App->externalManager->Load(filePath.c_str(), &bufferAux);
		if (bufferAux != nullptr) { newObject = DataLoading::LoadModel(App, bufferAux); }

		else { LOG("Model with path %s couldn't load.\n", path); }
		RELEASE_ARRAY(bufferAux);

	}

	return newObject;

}


bool ModelImporter::LoadNewModelComponents(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path) {

	aiScene* scene = (aiScene*)aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	aiMatrix4x4 trans;

	if (scene == nullptr || scene->HasMeshes() == false) {
		LOG("Error loading scene % s.\n", path);
		return false;
	}

	App->editorScene->AddGameObject(newObject);
	trans = scene->mRootNode->mTransformation;

	if (scene->mRootNode->mNumChildren != 0) { for (int i = 0; i < scene->mRootNode->mNumChildren; i++) { ModelImporter::LoadNewModelMesh(App, scene->mRootNode->mChildren[i], (aiScene*)scene, newObject, trans); } }

	aiReleaseImport(scene);

	LOG("Scene with path %s loaded.\n", path);

	return true;
}


void ModelImporter::LoadNewModelMesh(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform) {

	aiMatrix4x4 transform = accTransform * node->mTransformation;
	Mesh* mesh;
	Transform* transformation;

	if (node->mNumMeshes > 0) {

		for (int i = 0; i < node->mNumMeshes; i++) {

			long long int id = App->idGenerator.Int();

			GameObject* newObject = new GameObject(id, "NewGameObject", parent);

			mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);

			const aiMesh* paiMesh = (aiMesh*)scene->mMeshes[node->mMeshes[i]];
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {		// Vertices
				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[j]); //: &Zero3D	// There are the same normals as there are vertices, so we don't need a loop for them
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;	// Same as above

				mesh->vertices.push_back(pPos->x);
				mesh->vertices.push_back(pPos->y);
				mesh->vertices.push_back(pPos->z);
				mesh->textureCoord.push_back(pTexCoord->x);
				mesh->textureCoord.push_back(pTexCoord->y);
				mesh->normals.push_back(pNormal->x);
				mesh->normals.push_back(pNormal->y);
				mesh->normals.push_back(pNormal->z);
			}

			mesh->CalculateBoundingBoxes();
			LOG("New mesh with %d vertices.\n", mesh->vertices.size());

			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
				const aiFace& Face = paiMesh->mFaces[j];
				if (Face.mNumIndices != 3) { LOG("Not all faces of %s are triangles.\n", scene->mMeshes[node->mMeshes[i]]->mName.C_Str()); }
				mesh->indices.push_back(Face.mIndices[0]);
				mesh->indices.push_back(Face.mIndices[1]);
				mesh->indices.push_back(Face.mIndices[2]);
			}

			OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexId, mesh->vertices.size(), mesh->vertices.data());	// TODO: there is already a function in Mesh to do this... If we use it maybe we can kill OpenGLFunctionality header? :3
			OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsId, mesh->normals.size(), mesh->normals.data());
			OpenGLFunctionality::LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->textureCoordId, mesh->textureCoord.size(), mesh->textureCoord.data());
			OpenGLFunctionality::LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &mesh->indexId, mesh->indices.size(), mesh->indices.data());

			transformation = (Transform*)newObject->GetComponent(COMPONENT_TYPE::TRANSFORM);
			ModelImporter::aiTransformTofloat4x4Transform(transform, transformation);

			ModelImporter::LoadNewModelMaterial(App, scene, newObject, scene->mMeshes[node->mMeshes[i]]->mMaterialIndex);

			App->editorScene->AddGameObject(newObject);

		}

	}

	for (int i = 0; i < node->mNumChildren; i++) { LoadNewModelMesh(App, node->mChildren[i], scene, parent, transform); }

}


void ModelImporter::LoadNewModelMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId) {

	Material* material = nullptr;

	if (scene->mNumMaterials > 0) {

		const aiMaterial* pMaterial = scene->mMaterials[materialId];
		aiString Path;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

				std::string name;
				App->externalManager->SplitFilePath(Path.C_Str(), nullptr, &name);
				material = (Material*)newObject->AddComponent(COMPONENT_TYPE::MATERIAL);
				std::string FullPath = App->resourceManager->FindPathFromFileName(name);
				material->diffuseId = DataLoading::LoadTexture(App, FullPath.c_str());
				LOG("Material with id = %u loaded.\n", material->diffuseId);

			}

		}

		else if (scene->mNumMaterials < 2) { LOG("This model's texture is not specified as Diffuse."); }

	}

	else { LOG("This model has no materials attached.\n"); }

}


void ModelImporter::aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform) {

	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scale;

	matrix.Decompose(scale, rotation, position);

	transform->SetPosition(float3(position.x, position.y, position.z));
	transform->SetRotation(Quat(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->SetScale(float3(scale.x, scale.y, scale.z));	// TODO: I can optimize this by calling directly the recalculation matrix, but with a function that takes position, rotation and scale as values to recalculate it with

}


void ModelImporter::RecursiveChildCallToChangeID(Application* App, GameObject* gameObject) {

	gameObject->id = App->idGenerator.Int();
	if (gameObject->transform != nullptr) { gameObject->transform->id = App->idGenerator.Int(); }
	if (gameObject->mesh != nullptr) { gameObject->mesh->id = App->idGenerator.Int(); }
	if (gameObject->material != nullptr) { gameObject->material->id = App->idGenerator.Int(); }
	if (gameObject->camera != nullptr) { gameObject->camera->id = App->idGenerator.Int(); }
	for (int j = 0; j < gameObject->childs.size(); j++) { RecursiveChildCallToChangeID(App, gameObject->childs[j]); }

}







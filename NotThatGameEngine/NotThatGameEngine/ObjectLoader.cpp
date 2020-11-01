#include "ObjectLoader.h"


GameObject* LoadModel(Application* App, const char* path, const char* buffer, uint size, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(App->editorScene->GenerateId(), path, parent, enabled);	// TODO: convert path to only the name of the file with the FileSystem function :3

	if (buffer == nullptr && size == 0) { size = App->fileManager->Load(path, (char**)&buffer); }
	if (LoadScene(App, buffer, size, newObject, path)) {}
	else {
		delete newObject;
		newObject = nullptr;
	}

	RELEASE_ARRAY(buffer);

	return newObject;

}


bool LoadScene(Application* App, const char* buffer, uint size, GameObject* newObject, const char* path) {

	aiScene* scene = (aiScene*)aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	aiMatrix4x4 trans;

	if (scene == nullptr || scene->HasMeshes() == false) {
		LOG("Error loading scene % s.\n", path);
		return false;
	}

	App->editorScene->AddGameObject(newObject);
	trans = scene->mRootNode->mTransformation;

	if (scene->mRootNode->mNumChildren != 0) { for (int i = 0; i < scene->mRootNode->mNumChildren; i++) { LoadMeshNode(App, scene->mRootNode->mChildren[i], (aiScene*)scene, newObject, trans); } }

	aiReleaseImport(scene);

	LOG("Scene with path %s loaded.\n", path);

	return true;
}


uint LoadTexture(Application* App, const char* path, const char* buffer, uint size) {

	uint imageTest = App->texture->IsTextureRepeated(path);

	if (imageTest == 0) {

		if (buffer == nullptr && size == 0) { size = App->fileManager->Load(path, (char**)&buffer); }
		ILboolean ret;

		ilGenImages(1, &imageTest);
		ilBindImage(imageTest);

		ret = ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

		RELEASE_ARRAY(buffer);

		if (ret == IL_TRUE) { LOG("Texture with path %s loaded.\n", path); }
		else {
			LOG("Texture with path %s failed to load.\n", path);
			return 0;
		}

		LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
		ilDeleteImages(1, &imageTest);

		TextureData* texture = new TextureData(imageTest, path, GL_DIFFUSE, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		App->texture->AddTexture(texture);

	}

	return imageTest;

}


void LoadMeshNode(Application* App, aiNode* node, aiScene* scene, GameObject* parent, aiMatrix4x4 accTransform) {

	aiMatrix4x4 transform = accTransform * node->mTransformation;
	Mesh* mesh;
	Transform* transformation;

	if (node->mNumMeshes > 0) {

		for (int i = 0; i < node->mNumMeshes; i++) {

			GameObject* newObject = new GameObject(App->editorScene->GenerateId(), "NewGameObject", parent);

			mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);
			mesh->meshName = scene->mMeshes[node->mMeshes[i]]->mName.C_Str();

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

			LOG("New mesh %s with %d vertices.\n", mesh->meshName.c_str(), mesh->vertices.size());

			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
				const aiFace& Face = paiMesh->mFaces[j];
				if (Face.mNumIndices != 3) { LOG("Not all faces of %s are triangles.\n", scene->mMeshes[node->mMeshes[i]]->mName.C_Str()); }
				mesh->indices.push_back(Face.mIndices[0]);
				mesh->indices.push_back(Face.mIndices[1]);
				mesh->indices.push_back(Face.mIndices[2]);
			}

			LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexId, mesh->vertices.size(), mesh->vertices.data());
			LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsId, mesh->normals.size(), mesh->normals.data());
			LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->textureCoordId, mesh->textureCoord.size(), mesh->textureCoord.data());
			LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &mesh->indexId, mesh->indices.size(), mesh->indices.data());

			transformation = (Transform*)newObject->FindComponent(COMPONENT_TYPE::TRANSFORM);
			aiTransformTofloat4x4Transform(transform, transformation);

			LoadMeshMaterial(App, scene, newObject, scene->mMeshes[node->mMeshes[i]]->mMaterialIndex);

			App->editorScene->AddGameObject(newObject);

		}

	}

	for (int i = 0; i < node->mNumChildren; i++) { LoadMeshNode(App, node->mChildren[i], scene, parent, transform); }

}


void LoadMeshMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId) {

	Material* material = nullptr;

	if (scene->mNumMaterials > 0) {

		material = (Material*)newObject->AddComponent(COMPONENT_TYPE::MATERIAL);
		const aiMaterial* pMaterial = scene->mMaterials[materialId];
		aiString Path;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

				std::string FullPath = TEXTURES_PATH + App->fileManager->NormalizePath(Path.C_Str());	// TODO: do this better. It will need a function that iterates everything, yes. But do it
				material->diffuseId = LoadTexture(App, FullPath.c_str());
				LOG("Material with id = %u loaded.\n", material->diffuseId);

			}

		}

	}

	else { LOG("This model has no materials attached.\n"); }

}

void aiTransformTofloat4x4Transform(aiMatrix4x4 matrix, Transform* transform) {

	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scale;

	matrix.Decompose(scale, rotation, position);

	transform->SetPosition(float3(position.x, position.y, position.z));
	transform->SetRotation(Quat(rotation.x, rotation.y, rotation.z, rotation.w));
	transform->SetScale(float3(scale.x, scale.y, scale.z));	// TODO: I can optimize this by calling directly the recalculation matrix, but with a function that takes position, rotation and scale as values to recalculate it with

}







#include "ObjectLoader.h"
#include "Application.h"


GameObject* LoadModel(Application* App, const char* path, const char* buffer, uint size, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(App->editorScene->GenerateId(), path, parent, enabled);

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

	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene == nullptr || scene->HasMeshes() == false) {
		LOG("Error loading scene % s", path);
		return false;
	}

	Material* material = nullptr;

	LoadMesh(App, scene->mRootNode, newObject->components);

	// Init materials
	if (scene->mNumMaterials > 0) {

		material = (Material*)newObject->AddComponent(COMPONENT_TYPE::MATERIAL);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {	// TODO: We are supposing there will only be one material... Code is done in such way, so we have to limit Material creation in GameObjects in AddComponent()

			const aiMaterial* pMaterial = scene->mMaterials[i];

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string FullPath = TEXTURES_PATH + (std::string)Path.data;	// TODO: do this better. It will need a function that iterates everything, yes. But do it
					material->diffuseId = LoadTexture(App, FullPath.c_str());
				}
			}
		}
	}

	App->editorScene->AddGameObject(newObject);

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

		if (ret == IL_TRUE) {}
		else {
			LOG("Image with id: %u failed to load.\n", imageTest);
			return 0;
		}

		LoadGLTexture(&imageTest, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
		ilDeleteImages(1, &imageTest);

		TextureData* texture = new TextureData(imageTest, path, GL_DIFFUSE);

		App->texture->AddTexture(texture);

	}

	return imageTest;

}


Mesh* LoadMesh(Application* App, const aiNode* node, std::vector<Component*> vec) {

	Mesh* mesh = nullptr;

	aiVector3D pos, sca;
	aiQuaternion rot;

	node->mTransformation.Decompose(sca, rot, pos);

	float3 position (pos.x, pos.y, pos.z), scale (sca.x, sca.y, sca.z);
	Quat rotation(rot.x, rot.y, rot.z, rot.w);

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {

		const aiMesh* paiMesh = (aiMesh*)node->mMeshes[i];
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

		LOG("New mesh with %d vertices", mesh->vertices.size());

		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {		// Indices
			const aiFace& Face = paiMesh->mFaces[j];
			assert(Face.mNumIndices == 3);
			mesh->indices.push_back(Face.mIndices[0]);
			mesh->indices.push_back(Face.mIndices[1]);
			mesh->indices.push_back(Face.mIndices[2]);
		}

		LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->vertexId, mesh->vertices.size(), mesh->vertices.data());
		LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->normalsId, mesh->normals.size(), mesh->normals.data());
		LoadDataBufferFloat(GL_ARRAY_BUFFER, &mesh->textureCoordId, mesh->textureCoord.size(), mesh->textureCoord.data());
		LoadDataBufferUint(GL_ELEMENT_ARRAY_BUFFER, &mesh->indexId, mesh->indices.size(), mesh->indices.data());

	}

	return mesh;

}

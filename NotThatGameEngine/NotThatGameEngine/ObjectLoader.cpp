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

	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene == nullptr || scene->HasMeshes() == false) {
		LOG("Error loading scene % s", path);
		return false;
	}

	Mesh* mesh;
	Material* material = nullptr;
	std::vector<SceneObject> vec;
	aiMatrix4x4 trans;

	LoadMeshNode(scene->mRootNode, vec, trans);	// TODO: Am I loading this correctly? What is going on

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		mesh = (Mesh*)newObject->AddComponent(COMPONENT_TYPE::MESH);

		mesh->materialId = scene->mMeshes[i]->mMaterialIndex;

		const aiMesh* paiMesh = (aiMesh*)scene->mMeshes[i];
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

	aiReleaseImport(scene);
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

		TextureData* texture = new TextureData(imageTest, path, GL_DIFFUSE, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		App->texture->AddTexture(texture);

	}

	return imageTest;

}


void LoadMeshNode(aiNode* node, std::vector<SceneObject>& targetParent, aiMatrix4x4 accTransform) {

	aiMatrix4x4 transform;

	if (node->mNumMeshes > 0) {

		for (int i = 0; i < node->mNumMeshes; i++) {

			SceneObject* object = new SceneObject;
			object->meshID = node->mMeshes[i];
			object->transform = node->mTransformation * accTransform;
			targetParent.push_back(*object);
			
		}

	}

	transform = node->mTransformation * accTransform;

	for (int i = 0; i < node->mNumChildren; i++) { LoadMeshNode(node->mChildren[i], targetParent, transform); }

}








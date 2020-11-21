#include "ModelImporter.h"
#include "Save.h"
#include "Load.h"

GameObject* ModelImporter::LoadNewModel(Application* App, const char* path, const char* buffer, uint size, GameObject* parent, bool enabled) {

	GameObject* newObject = new GameObject(App->editorScene->GenerateId(), path, parent, enabled);	// TODO: convert path to only the name of the file with the FileSystem function :3

	if (buffer == nullptr && size == 0) { size = App->externalManager->Load(path, (char**)&buffer); }
	if (ModelImporter::LoadNewModelComponents(App, buffer, size, newObject, path)) {}
	else {
		delete newObject;
		newObject = nullptr;
	}

	RELEASE_ARRAY(buffer);

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

			GameObject* newObject = new GameObject(App->editorScene->GenerateId(), "NewGameObject", parent);

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

			transformation = (Transform*)newObject->FindComponent(COMPONENT_TYPE::TRANSFORM);
			ModelImporter::aiTransformTofloat4x4Transform(transform, transformation);

			ModelImporter::LoadNewModelMaterial(App, scene, newObject, scene->mMeshes[node->mMeshes[i]]->mMaterialIndex);

			App->editorScene->AddGameObject(newObject);

			DataSaving::SaveMesh(App, mesh);
		}

	}

	for (int i = 0; i < node->mNumChildren; i++) { LoadNewModelMesh(App, node->mChildren[i], scene, parent, transform); }

}


void ModelImporter::LoadNewModelMaterial(Application* App, aiScene* scene, GameObject* newObject, int materialId) {

	Material* material = nullptr;

	if (scene->mNumMaterials > 0) {

		material = (Material*)newObject->AddComponent(COMPONENT_TYPE::MATERIAL);
		const aiMaterial* pMaterial = scene->mMaterials[materialId];
		aiString Path;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

				std::string FullPath = "Textures/" + App->externalManager->NormalizePath(Path.C_Str());	// TODO: do this better. It will need a function that iterates everything, yes. But do it
				material->diffuseId = DataLoading::LoadTexture(App, FullPath.c_str());
				LOG("Material with id = %u loaded.\n", material->diffuseId);

			}

		}

		else if (scene->mNumMaterials < 2) { LOG("This model's texture is not specified as Diffuse."); }

		DataSaving::SaveMaterial(App, material);

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






#include "Application.h"
#include "Save.h"
#include "Load.h"
#include "ModelImporter.h"
#include "PathNode.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {}


ResourceManager::~ResourceManager() {}


bool ResourceManager::Init() {

	LoadLibraryFiles();

	return true;

}


bool ResourceManager::Start()
{

	App->eventManager->EventRegister(EVENT_ENUM::SAVE_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::LOAD_SCENE, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_DROPPED, this);
	App->eventManager->EventRegister(EVENT_ENUM::FILE_LOADED, this);

	return true;
}


update_status ResourceManager::PreUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::Update(float dt)
{

	return update_status::UPDATE_CONTINUE;

}


update_status ResourceManager::PostUpdate(float dt) {

	CheckListener(this);
	return update_status::UPDATE_CONTINUE;

}


bool ResourceManager::CleanUp() {

	return true;

}


void ResourceManager::LoadLibraryFiles() {

	PathNode loadingFilesNode = App->externalManager->GetAllFiles(LIBRARY_PATH);

	std::vector<std::string> sceneVec;
	App->externalManager->GetAllFilesWithExtension(ASSETS_PATH, EXTENSION_SCENES, sceneVec);

	std::vector<std::string> objectsVec = GetPathChildrenElements(loadingFilesNode);
	std::vector<std::string> textureVec;
	std::vector<std::string> componentVec;

	for (int i = 0; i < objectsVec.size(); i++) {

		ResourceEnum type = CheckResourceType(objectsVec[i]);
		if (type == ResourceEnum::SCENE) { sceneVec.push_back(objectsVec[i]); }
		else if (type == ResourceEnum::TEXTURE) { textureVec.push_back(objectsVec[i]); }
		else if (type == ResourceEnum::MATERIAL || type == ResourceEnum::MESH) { componentVec.push_back(objectsVec[i]); }

	}

	for (int i = 0; i < textureVec.size(); i++) { LoadResourceByType(textureVec[i], ResourceEnum::TEXTURE); }

	if (sceneVec.size() != 0) { LoadResourceByType(sceneVec[0], ResourceEnum::SCENE); }	// We only want to load one scene
	else {

		LOG("No scene to be loaded.\n");
		return;

	}

	for (int i = 0; i < objectsVec.size(); i++) { LoadResourceByType(objectsVec[i]); }

	// TODO: Load other components (camera)

	objectsVec.clear();
	sceneVec.clear();
	textureVec.clear();

}


std::vector<std::string> ResourceManager::GetPathChildrenElements(PathNode loadingNode) {

	std::vector<std::string> objectsVec;

	for (int i = 0; i < loadingNode.children.size(); i++) {

		if (loadingNode.children[i].isFile) { objectsVec.push_back(loadingNode.children[i].path); }
		else {

			std::vector<std::string> newVector = GetPathChildrenElements(loadingNode.children[i]);
			objectsVec.insert(objectsVec.end(), newVector.begin(), newVector.end());

		}

	}

	return objectsVec;

}


bool ResourceManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	std::string filePath;
	std::string extension;
	char* buffer;
	uint size;
	uint id;
	ResourceEnum type;

	switch (eventId) {

	case EVENT_ENUM::SAVE_SCENE:

		DataSaving::SaveScene(App);

		break;

	case EVENT_ENUM::LOAD_SCENE:

		LoadLibraryFiles();

		break;

	case EVENT_ENUM::FILE_DROPPED:
	case EVENT_ENUM::FILE_LOADED:

		filePath = (char*)var;
		filePath = App->externalManager->NormalizePath(filePath.c_str());
		filePath = App->externalManager->LocalizePath(filePath);
		size = App->externalManager->Load(filePath.c_str(), &buffer);

		type = CheckResourceType(filePath);

		if (size != 0) {

			switch (type) {

			case ResourceEnum::EXTERNAL_MODEL:

				ModelImporter::LoadNewModel(App, filePath.c_str(), buffer, size);
				break;

			case ResourceEnum::TEXTURE:

				id = DataLoading::LoadTexture(App, filePath.c_str(), buffer, size);
				App->eventManager->GenerateEvent(EVENT_ENUM::PUT_TEXTURE_TO_FOCUSED_MODEL, EVENT_ENUM::NULL_EVENT, (void*)id);
				break;

			case ResourceEnum::SCENE:

				DataLoading::LoadScene(App, buffer);

				break;

			case ResourceEnum::UNKNOWN:
			
				LOG("File with path %s, with readable format, has been dropped into the engine.", filePath.c_str());
				break;

			default:
				break;
			}

			return true;
		}
	}

}

ResourceEnum ResourceManager::CheckResourceType(std::string name) {

	std::string ext;

	App->externalManager->SplitFilePath(name.c_str(), nullptr, nullptr, &ext);

	ext = "." + ext;

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 7, "Code Needs Update");

	if (ext == EXTENSION_SCENES) { return ResourceEnum::SCENE; }
	else if (ext == EXTENSION_MODELS) { return ResourceEnum::OWN_MODEL; }
	else if (ext == EXTENSION_MESHES) { return ResourceEnum::MESH; }
	else if (ext == EXTENSION_MATERIALS) { return ResourceEnum::MATERIAL; }
	else if (ext == ".FBX" || ext == ".fbx" || ext == ".OBJ" || ext == ".obj") { return ResourceEnum::EXTERNAL_MODEL; }
	else if (ext == ".tga" || ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".TGA" || ext == ".PNG" || ext == ".JPG" || ext == ".DDS" || ext == EXTENSION_TEXTURES) { return ResourceEnum::TEXTURE; }

	return ResourceEnum::UNKNOWN;

}


void ResourceManager::LoadResourceByType(std::string name, ResourceEnum type) {

	ResourceEnum resourceType;
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	char* buffer = nullptr;
	std::string fileName;

	type == ResourceEnum::NONE ? resourceType = CheckResourceType(name) : resourceType = type;

	switch (resourceType) {

	case ResourceEnum::SCENE:

		App->externalManager->Load(name.c_str(), &buffer);
		DataLoading::LoadScene(App, buffer);

		break;

	case ResourceEnum::OWN_MODEL:



		break;

	case ResourceEnum::MESH:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		mesh = (Mesh*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (mesh != nullptr) {

			App->externalManager->Load(name.c_str(), &buffer);
			DataLoading::LoadMesh(buffer, mesh);

		}

		break;

	case ResourceEnum::MATERIAL:

		App->externalManager->SplitFilePath(name.c_str(), nullptr, &fileName);
		material = (Material*)App->editorScene->FindGameObjectByComponent((long long int)std::stoi(fileName));

		if (material != nullptr) {

			App->externalManager->Load(name.c_str(), &buffer);
			DataLoading::LoadMaterial(App, buffer, material);

		}

		break;

	case ResourceEnum::TEXTURE:

		DataLoading::LoadTexture(App, name.c_str());

		break;

	case ResourceEnum::UNKNOWN:

		LOG("Unknown resource type for file %s.\n", name.c_str());

		break;

	default:
		break;
	}

	RELEASE_ARRAY(buffer);

}



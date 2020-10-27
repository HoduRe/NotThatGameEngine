#include "Application.h"
#include "PathNode.h"
#include "ObjectLoader.h"

#include "PhysFS/include/physfs.h"
#include <fstream>
#include <filesystem>

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )


FileManager::FileManager(Application* app, bool start_enabled) : Module(app, start_enabled) {

	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	if (PHYSFS_setWriteDir(".") == 0) { LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError()); }

	AddPath(".");	// Those must NOT be joined
	AddPath("Assets");

	std::string name = PHYSFS_getBaseDir();

	CreateDir(LIBRARY_PATH);
	CreateDir(FOLDERS_PATH);
	CreateDir(MESHES_PATH);
	CreateDir(TEXTURES_PATH);
	CreateDir(MATERIALS_PATH);
	CreateDir(MODELS_PATH);
	CreateDir(ANIMATIONS_PATH);
	CreateDir(PARTICLES_PATH);
	CreateDir(SHADERS_PATH);
	CreateDir(SCENES_PATH);
}


FileManager::~FileManager() { PHYSFS_deinit(); }


bool FileManager::Init() {

	bool ret = true;

	App->eventManager->EventRegister(EVENT_ENUM::FILE_DROPPED, this);

	return ret;
}


bool FileManager::CleanUp() { return true; }


update_status FileManager::PreUpdate(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


update_status FileManager::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status FileManager::PostUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


bool FileManager::ExecuteEvent(EVENT_ENUM eventId, void* var) {

	std::string filePath;
	char* buffer;
	uint size;
	ResourceEnum type;

	switch (eventId) {

	case EVENT_ENUM::FILE_DROPPED:

		filePath = (char*)var;
		filePath = NormalizePath(filePath.c_str());
		filePath = LocalizePath(filePath);
		size = Load(filePath.c_str(), &buffer);

		type = CheckResourceType(filePath);
		
		if (size != 0) {
			switch (type) {
			case ResourceEnum::MODEL:
				LoadModel(App, filePath.c_str(), buffer, size);
				break;

			case ResourceEnum::TEXTURE:
				LoadTexture(App, filePath.c_str(), buffer, size);
				break;

			default:
				break;
			}
		}

		break;
	default:
		break;
	}

	return false;

}


bool FileManager::AddPath(const char* path_or_zip) {

	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0) { LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError()); }
	else { ret = true; }

	return ret;

}

// Check if a file exists
bool FileManager::Exists(const char* file) const { return PHYSFS_exists(file) != 0; }


bool FileManager::CreateDir(const char* dir) {

	if (IsDirectory(dir) == false) {

		PHYSFS_mkdir(dir);
		return true;

	}

	return false;
}


bool FileManager::IsDirectory(const char* file) const { return PHYSFS_isDirectory(file) != 0; }


const char* FileManager::GetWriteDir() const { return PHYSFS_getWriteDir(); }


void FileManager::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const {

	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++) {

		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str())) { dir_list.push_back(*i); }
		else { file_list.push_back(*i); }

	}

	PHYSFS_freeList(rc);

}


void FileManager::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const {

	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++) {

		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);
		if (ext == extension) { file_list.push_back(files[i]); }

	}

}

PathNode FileManager::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const {

	PathNode root;
	if (Exists(directory)) {

		root.path = directory;
		SplitFilePath(directory, nullptr, &root.localPath);

		if (root.localPath == "") { root.localPath = directory; }

		std::vector<std::string> file_list, dir_list;
		DiscoverFiles(directory, file_list, dir_list);

		//Adding all child directories
		for (uint i = 0; i < dir_list.size(); i++) {

			std::string str = directory;
			str.append("/").append(dir_list[i]);
			root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));

		}

		//Adding all child files
		for (uint i = 0; i < file_list.size(); i++) {

			bool filter = true, discard = false;

			if (filter_ext != nullptr) { filter = HasExtension(file_list[i].c_str(), *filter_ext); }

			if (ignore_ext != nullptr) { discard = HasExtension(file_list[i].c_str(), *ignore_ext); }

			if (filter == true && discard == false) {

				std::string str = directory;
				str.append("/").append(file_list[i]);
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));

			}
		}

		root.isFile = HasExtension(root.path.c_str());
		root.isLeaf = root.children.empty() == true;

	}
	return root;
}


void FileManager::GetRealDir(const char* path, std::string& output) const {

	output = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	output.append(*PHYSFS_getSearchPath()).append("/");
	output.append(PHYSFS_getRealDir(path)).append("/").append(path);

}


std::string FileManager::GetPathRelativeToAssets(const char* originalPath) const {

	std::string ret;
	GetRealDir(originalPath, ret);
	return ret;

}


bool FileManager::HasExtension(const char* path) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";

}


bool FileManager::HasExtension(const char* path, std::string extension) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;

}


bool FileManager::HasExtension(const char* path, std::vector<std::string> extensions) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "") { return true; }
	for (uint i = 0; i < extensions.size(); i++) { if (extensions[i] == ext) { return true; } }
	return false;

}


std::string FileManager::NormalizePath(const char* full_path) const {

	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i) { if (newPath[i] == '\\') { newPath[i] = '/'; } }
	return newPath;

}


std::string FileManager::LocalizePath(std::string path) const {

	std::string newPath;
	std::string dirPath = PHYSFS_getBaseDir();
	std::string obliguedPath = OBLIGUED_PATH;
	dirPath = NormalizePath(dirPath.c_str());

	int size = path.size(), i = 0, j = 0;
	bool directorySkipped = false;
	char char1, char2;

	while (path[j] == dirPath[j] && j < size) { j++; }
	while (path[j] != obliguedPath[0] && j < size) { j++; }
	while (path[j] == obliguedPath[i] && j < size) { j++; i++; }

	for (j; j < size; j++) { newPath.push_back(path[j]); }

	return newPath;

}


void FileManager::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const {

	if (full_path != nullptr) {

		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr) {

			if (pos_separator < full.length()) { *path = full.substr(0, pos_separator + 1); }
			else { path->clear(); }

		}

		if (file != nullptr) {

			if (pos_separator < full.length()) { *file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1); }
			else { *file = full.substr(0, pos_dot); }

		}

		if (extension != nullptr) {

			if (pos_dot < full.length()) { *extension = full.substr(pos_dot + 1); }
			else { extension->clear(); }

		}
	}

}


unsigned int FileManager::Load(const char* path, const char* file, char** buffer) const {

	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);

}


uint FileManager::Load(const char* file, char** buffer) const {	// Read a whole file and put it in a new buffer

	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr) {

		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

		if (size > 0) {

			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);

			if (readed != size) {

				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);

			}

			else {

				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';

			}
		}

		if (PHYSFS_close(fs_file) == 0) { LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError()); }
	}

	else { LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError()); }

	return ret;

}


bool FileManager::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath) {

	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}


bool FileManager::DuplicateFile(const char* srcFile, const char* dstFile) {

	//TODO: Compare performance to calling Load(srcFile) and then Save(dstFile)
	std::ifstream src;
	src.open(srcFile, std::ios::binary);
	bool srcOpen = src.is_open();
	std::ofstream  dst(dstFile, std::ios::binary);
	bool dstOpen = dst.is_open();

	dst << src.rdbuf();

	src.close();
	dst.close();

	if (srcOpen && dstOpen) {
		LOG("[success] File Duplicated Correctly");
		return true;
	}

	else {
		LOG("[error] File could not be duplicated");
		return false;
	}

}


int close_sdl_rwops(SDL_RWops* rw) {

	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;

}


uint FileManager::Save(const char* file, const void* buffer, unsigned int size, bool append) const {	// Save a whole buffer to disk

	unsigned int ret = 0;
	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr) {

		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size) { LOG("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError()); }

		else {

			if (append == true) { LOG("Added %u data to [%s%s]", size, GetWriteDir(), file); }
			else if (overwrite == true) { LOG("File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size); }
			else { LOG("New file created [%s%s] of %u bytes", GetWriteDir(), file, size); }
			ret = written;

		}

		if (PHYSFS_close(fs_file) == 0) { LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError()); }
	}

	else { LOG("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError()); }

	return ret;
}


bool FileManager::Remove(const char* file) {

	bool ret = false;

	if (file != nullptr) {

		if (IsDirectory(file)) {

			std::vector<std::string> containedFiles, containedDirs;
			PathNode rootDirectory = GetAllFiles(file);
			for (uint i = 0; i < rootDirectory.children.size(); ++i) { Remove(rootDirectory.children[i].path.c_str()); }

		}

		if (PHYSFS_delete(file) != 0) {

			LOG("File deleted: [%s]", file);
			ret = true;

		}

		else { LOG("File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError()); }
	}

	return ret;
}


uint64 FileManager::GetLastModTime(const char* filename) { return PHYSFS_getLastModTime(filename); }


std::string FileManager::GetUniqueName(const char* path, const char* name) const {

	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> files, dirs;
	DiscoverFiles(path, files, dirs);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i) {

		unique = true;

		if (i > 0) {

			finalName = std::string(name).append("_");
			if (i < 10) { finalName.append("0"); }
			finalName.append(std::to_string(i));

		}

		for (uint f = 0; f < files.size(); ++f) {

			if (finalName == files[f]) {

				unique = false;
				break;

			}

		}
	}

	return finalName;

}


ResourceEnum FileManager::CheckResourceType(std::string name) {

	std::string ext;

	SplitFilePath(name.c_str(), nullptr, nullptr, &ext);

	static_assert(static_cast<int>(ResourceEnum::UNKNOWN) == 3, "Code Needs Update");

	if (ext == "FBX" || ext == "fbx") { return ResourceEnum::MODEL; }
	else if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "TGA" || ext == "PNG" || ext == "JPG" || ext == "dds") { return ResourceEnum::TEXTURE; }

	return ResourceEnum::UNKNOWN;

}





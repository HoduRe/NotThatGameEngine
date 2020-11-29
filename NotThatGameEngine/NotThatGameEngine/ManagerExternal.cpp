#include "Application.h"
#include "ManagerExternal.h"
#include "PathNode.h"
#include "Import.h"

#include "SDL/include/SDL.h"
#include "PhysFS/include/physfs.h"
#include <fstream>
#include <filesystem>

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#ifdef _DEBUG
#define _PATH_BOOL true
#else
#define _PATH_BOOL false
#endif

ExternalManager::ExternalManager(Application* app, bool start_enabled) : Module(app, start_enabled) {

	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	LOG("PhysFS initialized.\n");
	
	std::string name = PHYSFS_getBaseDir();
	LOG("Base directory: %s.\n", name.c_str());

	if (_PATH_BOOL == false) {
		
		if (PHYSFS_setWriteDir(".") == 0) { LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError()); }
		AddPath(".");

	}

	else {

		if (PHYSFS_setWriteDir(".") == 0) { LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError()); }

		AddPath(".");	// Those must NOT be joined
		AddPath("Assets");

	}

	name = PHYSFS_getWriteDir();
	LOG("Write directory: %s.\n", name.c_str());

	CreateDir(LIBRARY_PATH);
	CreateDir(FOLDERS_PATH);
	CreateDir(MESHES_PATH);
	CreateDir(CAMERAS_PATH);
	CreateDir(TEXTURES_PATH);
	CreateDir(MATERIALS_PATH);
	CreateDir(MODELS_PATH);
	CreateDir(ANIMATIONS_PATH);
	CreateDir(PARTICLES_PATH);
	CreateDir(SHADERS_PATH);
	CreateDir(LIBRARY_SCENES_PATH);
	CreateDir(SCENES_PATH);

}


ExternalManager::~ExternalManager() { PHYSFS_deinit(); }


bool ExternalManager::Init() {

	bool ret = true;

	return ret;
}


bool ExternalManager::CleanUp() { return true; }


update_status ExternalManager::PreUpdate(float dt) {

	CheckListener(this);

	return update_status::UPDATE_CONTINUE;
}


update_status ExternalManager::Update(float dt) {

	return update_status::UPDATE_CONTINUE;
}


update_status ExternalManager::PostUpdate(float dt) {

	return update_status::UPDATE_CONTINUE;
}


bool ExternalManager::AddPath(const char* path_or_zip) {

	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0) { LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError()); }
	else { ret = true; }

	return ret;

}

// Check if a file exists
bool ExternalManager::Exists(const char* file) const { return PHYSFS_exists(file) != 0; }


bool ExternalManager::CreateDir(const char* dir) {

	if (IsDirectory(dir) == false) {

		PHYSFS_mkdir(dir);
		return true;

	}

	return false;
}


bool ExternalManager::IsDirectory(const char* file) const { return PHYSFS_isDirectory(file) != 0; }


const char* ExternalManager::GetWriteDir() const { return PHYSFS_getWriteDir(); }


void ExternalManager::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const {

	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++) {

		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str())) { dir_list.push_back(*i); }
		else { file_list.push_back(*i); }

	}

	PHYSFS_freeList(rc);

}


void ExternalManager::DiscoverFiles(const char* directory, std::vector<std::string>& file_list) const {

	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++) {

		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (!IsDirectory(str.c_str())) { file_list.push_back(*i); }

	}

	PHYSFS_freeList(rc);

}


void ExternalManager::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const {

	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++) {

		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);
		if ("." + ext == extension) { file_list.push_back(files[i]); }

	}

}

PathNode ExternalManager::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const {

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


void ExternalManager::GetRealDir(const char* path, std::string& output) const {

	output = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	output.append(*PHYSFS_getSearchPath()).append("/");
	output.append(PHYSFS_getRealDir(path)).append("/").append(path);

}


std::string ExternalManager::GetPathRelativeToAssets(const char* originalPath) const {

	std::string ret;
	GetRealDir(originalPath, ret);
	return ret;

}


bool ExternalManager::HasExtension(const char* path) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";

}


bool ExternalManager::HasExtension(const char* path, std::string extension) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;

}


bool ExternalManager::HasExtension(const char* path, std::vector<std::string> extensions) const {

	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "") { return true; }
	for (uint i = 0; i < extensions.size(); i++) { if (extensions[i] == ext) { return true; } }
	return false;

}


std::string ExternalManager::NormalizePath(const char* full_path) const {

	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i) { if (newPath[i] == '\\') { newPath[i] = '/'; } }
	return newPath;

}


std::string ExternalManager::LocalizePath(std::string path) const {

	std::string newPath;
	std::string dirPath = PHYSFS_getBaseDir();
	dirPath = NormalizePath(dirPath.c_str());
	std::string obliguedPath;

	if (_PATH_BOOL) { obliguedPath = ASSETS_PATH + (std::string)"/"; }

	int size = path.size(), i = 0, j = 0;
	bool directorySkipped = false;
	char char1, char2;

	while (path[j] == dirPath[j] && j < size) { j++; }
	if (obliguedPath.size() != 0) {
		while (path[j] != obliguedPath[0] && j < size) { j++; }
		while (path[j] == obliguedPath[i] && j < size) { j++; i++; }
	}

	for (j; j < size; j++) { newPath.push_back(path[j]); }

	if (newPath.size() == 0) { newPath = path; }	// If newPath has nothing, path may already be localized

	return newPath;

}


void ExternalManager::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const {

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


unsigned int ExternalManager::Load(const char* path, const char* file, char** buffer) const {

	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);

}


uint ExternalManager::Load(const char* file, char** buffer) const {	// Read a whole file and put it in a new buffer

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


bool ExternalManager::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath) {

	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}


bool ExternalManager::DuplicateFile(const char* srcFile, const char* dstFile) {

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
		LOG("[success] File Duplicated Correctly.\n");
		return true;
	}

	else {
		LOG("[error] File could not be duplicated.\n");
		return false;
	}

}


int close_sdl_rwops(SDL_RWops* rw) {

	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;

}


uint ExternalManager::Save(const char* path, const void* buffer, unsigned int size, bool append) const {	// Save a whole buffer to disk

	unsigned int ret = 0;
	bool overwrite = PHYSFS_exists(path) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(path) : PHYSFS_openWrite(path);

	if (fs_file != nullptr) {

		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size) { LOG("[error] File System error while writing to file %s: %s.\n", path, PHYSFS_getLastError()); }

		else {

			if (append == true) { LOG("Added %u data to [%s%s].\n", size, GetWriteDir(), path); }
			else if (overwrite == true) { LOG("File [%s%s] overwritten with %u bytes.\n", GetWriteDir(), path, size); }
			else { LOG("New file created [%s%s] of %u bytes.\n", GetWriteDir(), path, size); }
			ret = written;

		}

		if (PHYSFS_close(fs_file) == 0) { LOG("[error] File System error while closing file %s: %s.\n", path, PHYSFS_getLastError()); }
	}

	else { LOG("[error] File System error while opening file %s: %s.\n", path, PHYSFS_getLastError()); }

	return ret;
}


bool ExternalManager::RemoveDirectoryByName(const char* path) {

	bool ret = false;

	if (path != nullptr) {

		if (IsDirectory(path)) {

			std::vector<std::string> containedFiles, containedDirs;
			PathNode rootDirectory = GetAllFiles(path);
			for (uint i = 0; i < rootDirectory.children.size(); ++i) { RemoveDirectoryByName(rootDirectory.children[i].path.c_str()); }

		}

		if (PHYSFS_delete(path) != 0) {

			LOG("File deleted: [%s].\n", path);
			ret = true;

		}

		else { LOG("File System error while trying to delete [%s]: %s.\n", path, PHYSFS_getLastError()); }
	}

	return ret;
}


bool ExternalManager::RemoveFileByName(const char* path) {

	bool ret = false;

	if (path != nullptr) {

		if (PHYSFS_delete(path) != 0) {

			LOG("File deleted: [%s].\n", path);
			ret = true;

		}

		else { LOG("File System error while trying to delete [%s]: %s.\n", path, PHYSFS_getLastError()); }
	}

	return ret;
}


uint64 ExternalManager::GetLastModTime(const char* filePath) { return PHYSFS_getLastModTime(filePath); }


std::string ExternalManager::GetUniqueName(const char* path, const char* name) const {

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





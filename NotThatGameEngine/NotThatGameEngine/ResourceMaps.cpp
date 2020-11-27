#include "ManagerResource.h"

FileInfo::FileInfo() : filePath(), id(), lastTimeChanged(), checked(false) {}


FileInfo::FileInfo(std::string _filePath, int _id, int _lastChange) : filePath(_filePath), id(_id), lastTimeChanged(_lastChange), checked(true) {}


FileInfo::~FileInfo() {}


LibraryInfo::LibraryInfo() : filePath(), type() {}


LibraryInfo::LibraryInfo(std::string _filePath, ResourceEnum _type) : filePath(_filePath), type(_type) {}


LibraryInfo::~LibraryInfo() {}


MemoryInfo::MemoryInfo() : libraryPath() {}


MemoryInfo::~MemoryInfo() {}




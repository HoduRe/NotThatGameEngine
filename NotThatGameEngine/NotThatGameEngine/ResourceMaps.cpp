#include "ManagerResource.h"

FileInfo::FileInfo() : fileName(), id(), lastTimeChanged(), type(), checked(false) {}


FileInfo::~FileInfo() {}


LibraryInfo::LibraryInfo() : fileName(), type() {}


LibraryInfo::~LibraryInfo() {}


MemoryInfo::MemoryInfo() : libraryPath() {}


MemoryInfo::~MemoryInfo() {}




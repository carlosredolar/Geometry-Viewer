#ifndef __ModuleFileManager_H__
#define __ModuleFileManager_H__

#include "Module.h"
#include "Libs/SDL/include/SDL.h"
#include "Defs.h"
#include "Libs/PhysFS/include/physfs.h"
#include "PathNode.h"

#include <vector>
#include <string>

#pragma comment( lib, "Libs/PhysFS/libx86/physfs.lib" )

struct SDL_RWops;
struct aiFileIO;
class Config;

namespace FileManager
{
	//public:

	//FileManager(bool start_enabled = true);
	//~FileManager();

	// Called before render is available
	bool Init();
	// Called before quitting
	bool CleanUp();

	void CreateFolderDirs();

	// Return the bytes of a PhysFS filehandle
	//uint Load(const char* path, char** buffer) ;

	// Allows you to use pointers to memory instead of files or things such as images or samples
	SDL_RWops* Load(const char* path);

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file);
	//bool ExistsFile(const char* file, const char* ext);
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file);

	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list);

	//std::string GetInternalFolder(const char* ext);

	bool HasExtension(const char* path);
	bool HasExtension(const char* path, std::string extension);
	bool HasExtension(const char* path, std::vector<std::string> extensions);

	std::string NormalizePath(const char* path);
	std::string LowerCaseString(const char* path);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr);

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer);
	unsigned int Load(const char* file, char** buffer) ;

	//bool ImportFile(const char* file, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false);
	bool Remove(const char* file);
	bool Delete(const char* file);
	bool Rename(const char* old_name, const char* new_name);


	// GET - - -
	void			GetRealDir(const char* path, std::string& output);
	void			GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list);
	const char*		GetWriteDir();
	uint64			GetLastModTime(const char* filename);
	std::string		GetUniqueName(const char* path, const char* name);
	//std::string		GetExtensionFolder(const char* fileExtension);
	std::string		GetPathRelativeToAssets(const char* originalPath);
	PathNode		GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr);
	std::string		ExtractFileName(const char* fullPath);
	std::string		ExtractFileNameAndExtension(const char* fullPath);
	std::string		ExtractFileExtension(const char* fullPath);

	//private:
	int CheckPath(const char*);

	//private:
	//std::vector<std::string>	pathVector;
	//std::vector<char*>		bufferVector;
	//std::vector<uint>			bytesVector;
};

#endif // __ModuleFileManager_H_

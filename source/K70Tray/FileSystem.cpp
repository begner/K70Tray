#include "stdafx.h"
#include "FileSystem.h"


FileSystem::FileSystem()
{
}


FileSystem::~FileSystem()
{
}

string FileSystem::getPath() {
	return sCompletePath;
}

string FileSystem::getCwd() {
	wchar_t* cwd;
	cwd = _wgetcwd(NULL, 0);
	string sCwd = ws2s((wstring)cwd) + "\\";
	return sCwd;
}

vector<string> FileSystem::GetFilesInPath(string path, bool relativeToCWD) {
	return FileSystem::GetFilesInPath(path, relativeToCWD, "*.*");
}

vector<string> FileSystem::GetFilesInPath(string path, bool relativeToCWD, string filemask) {
	return FindInFolders("file", path, relativeToCWD, filemask);
}

vector<string> FileSystem::GetFoldersInPath(string path, bool relativeToCWD) {
	return FindInFolders("folder", path, relativeToCWD, "*.*");
}

bool FileSystem::FileExists(const std::string& name) {
	ifstream f(name.c_str());
	if (f.good()) {
		f.close();
		return true;
	}
	else {
		f.close();
		return false;
	}
}


vector<string> FileSystem::FindInFolders(string type, string path, bool relativeToCWD, string filemask) {
	vector<string> ret;

	string sCwd = "";

	if (relativeToCWD) {
		sCwd = getCwd();
	}

	sCompletePath = sCwd + path + "\\";
	string sSearchPath = sCompletePath + filemask;
	wstring wsCompletePath = s2ws(sSearchPath);
	const wchar_t * sDir = wsCompletePath.c_str();

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;


	DebugMsg("Search for Directories in: '%s'", sSearchPath.c_str());

	if ((hFind = FindFirstFile(sDir, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		DebugMsg("Path not found: [%s]", ws2s(sDir).c_str());
		return ret;
	}
	do
	{
		//Find first file will always return "."
		//    and ".." as the first two directories. 
		if (wcscmp(fdFile.cFileName, L".") != 0
			&& wcscmp(fdFile.cFileName, L"..") != 0)
		{
			if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{
				if (type == "folder") {
					string dirName = ws2s(wstring(fdFile.cFileName));
					// DebugMsg("Directory found: '%s'", dirName.c_str());
					ret.push_back(dirName);
				}
			}
			else {
				if (type == "file") {
					string fileName = ws2s(wstring(fdFile.cFileName));
					// DebugMsg("File found: '%s'", fileName.c_str());
					ret.push_back(fileName);
				}
			}
		}
		
	} while (FindNextFile(hFind, &fdFile)); //Find the next file. 

	FindClose(hFind); 

	return ret;
}


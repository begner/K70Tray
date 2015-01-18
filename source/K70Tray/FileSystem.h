#pragma once
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include "convert.h"
#include "globals.h"

using namespace std;

class FileSystem
{
private:
	string sCompletePath;
	
	vector<string> FindInFolders(string type, string path, bool relativeToCWD, string filter);
public:
	FileSystem();
	~FileSystem();
	string getCwd();
	vector<string> GetFoldersInPath(string path, bool relativeToCWD);
	vector<string> GetFilesInPath(string path, bool relativeToCWD);
	vector<string> GetFilesInPath(string path, bool relativeToCWD, string filemask);
	bool FileExists(const std::string& name);
	string getPath();
};



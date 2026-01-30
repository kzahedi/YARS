#ifndef __YARS_FILESYSTEM_OPERATIONS_H__
#define __YARS_FILESYSTEM_OPERATIONS_H__

#include <yars/util/YarsErrorHandler.h>

#include <filesystem>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

class FileSystemOperations
{
  public:
  static bool doesDirExist(fs::path dirPath);
  static bool doesFileExist(fs::path filePath);
  static bool doesDirExist(string *dirName);
  static bool doesFileExist(string *fileName);
  static bool doesExecutableExist(string exe);
  static void createDir(string dirName);

  static string* getFirstExistingDirContainingDir(std::vector<string> dirs,
      string *containedDirName);
  static string* getFirstExistingDirContainingFile(std::vector<string> dirs,
      string *containedFileName);
  static string* getFirstExistingDir(std::vector<string> dirs);
  static string* getFirstExistingFile(std::vector<string> files);

  static void checkValidPath(string *name, bool isDir, bool fatal, string
      description);
  static void checkValidPathFromAlternatives(string *name, string *pathName,
      std::vector<string> *pathCandidates, bool fatal, string description);

  protected:
    FileSystemOperations();
    ~FileSystemOperations();
};

#endif

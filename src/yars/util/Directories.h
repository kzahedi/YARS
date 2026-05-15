#ifndef __YARS_DIRECTORIES_H__
#define __YARS_DIRECTORIES_H__

#include <filesystem>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#define ROBOT_CONTROLLER_PREFIX "libYarsController"

#ifndef __APPLE__
#define DYNAMIC_LIB_SUFFIX ".so"
#else // __APPLE__
#define DYNAMIC_LIB_SUFFIX ".dylib"
#endif // __APPLE__

namespace fs = std::filesystem;
using namespace std;

class Directories
{
  public:
    Directories();
    bool xsd(string *xsdPathResult);
    bool library(string *result, string library, string path = "");
    bool ply(string *result, string filename, string path = "");
    bool configFile(string *result);

    bool getPly(string *result, string filename);
    bool getController(string *result, string filename);

    bool checkXsdPath(string xsdPath);
    bool checkLibrariesPath(string path, string library = "");
    bool checkPlyPath(string path, string file = "");

    static bool doesFileExist(string file);

    void getApplicationDirectory(string *s, char *argv);
    void getUniqueDirectoryName(string *name);
    void getFileNameDateTime(string *name);

    void setFullPath(string *dir);

    /// Prepend a user-supplied path to the controller-library search list.
    /// Adds both `<path>` and `<path>/lib` so the call site doesn't have to
    /// know which form the user gave. Called after CLI parsing completes,
    /// not from the constructor, to avoid an init cycle through
    /// YarsConfiguration::instance().
    void addLibraryPath(const string &path);

    // **************************************************************************
    // Debugging
    // **************************************************************************
    void toString(string prefix, string *returnString);

  private:

    // **************************************************************************
    // Directory and file check functions
    // **************************************************************************
    bool __doesDirectoryExist(fs::path directoryPath);
    static bool __doesFileExist(fs::path filePath);

    // **************************************************************************
    // Retrieve file from a list of directories
    // **************************************************************************
    bool __getFirstExistingDirContainingFile(std::vector<string> directories,
        string *containedFileName, string *path);

    // **************************************************************************
    // Retrieve directory containing a set of files
    // **************************************************************************
    bool __getFirstExistingDirContainingFiles(std::vector<string> dirs, std::vector<string> filenames, string *texturePath);

    // **************************************************************************
    // Setup directories
    // **************************************************************************
    void __setupXsdDirectories();
    void __setupLibsDirectories();
    void __setupConfigDirectories();
    void __setupMaterialsDirectories();
    void __setupPlyDirectories();

    // **************************************************************************
    // Find directories
    // **************************************************************************
    void __findXsdDirectory();
    void __findMaterialsDirectory();


    // **************************************************************************
    // Variables
    // **************************************************************************
    std::vector<string> _xsdPathCandidates;
    std::vector<string> _libPathCandidates;
    std::vector<string> _plyPathCandidates;
    std::vector<string> _configFilePathCandidate;
    string         _captureFramesDirectory;
    bool           _xsdFound;
    string         _xsdDirectory;
    bool           _materialsFound;
    string         _materialsDirectory;
};

#endif // __YARS_DIRECTORIES_H__

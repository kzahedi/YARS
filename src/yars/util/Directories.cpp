#include <yars/util/Directories.h>

#include <yars/util/StringTokeniser.h>
#include <yars/util/Timer.h>
#include <yars/util/FileSystemOperations.h>

#define YARS_DIR_ROSIML_NAME "RoSiML.xsd"
#define YARS_CONFIG_FILE_NAME ".yarsrc"
#define YARS_GLOBAL_CONFIG_FILE_NAME "yarsrc"
#define YARS_GLOBAL_CONFIG_FILE_DIR "/etc/"

#define YARS_DIR_LOCAL_YARS_DIR ".yars"
#define YARS_DIR_XSD_DIR "xsd"
#define YARS_DIR_CURRENT_DIR ((string) ".")
#define YARS_DIR_LIB_DIR "lib"

#define YARS_DIR_PLY_DIR "ply"

#ifdef __APPLE__
#define YARS_DIR_DELIMITER "/"
#define YARS_INSTALL_SUFFIX "/yars"
#endif

#ifdef __linux__
#define YARS_DIR_DELIMITER "/"
#define YARS_INSTALL_SUFFIX "/yars"
#endif

#ifdef WIN32
#define YARS_DIR_DELIMITER "\\"
#define YARS_INSTALL_SUFFIX "\\yars"
#endif

using namespace std;

#include <iostream>

/** \brief Private constructor.
 *
 * Calls the set-up and find functions
 * \see Directories::__setupXsdDirectories
 * \see Directories::__setupLibsDirectories
 * \see Directories::__findXsdDirectory
 */
Directories::Directories()
{
  _xsdFound = false;

  __setupXsdDirectories();
  __setupLibsDirectories();
  __setupPlyDirectories();
  __setupConfigDirectories();

  // __findXsdDirectory();
}

/** \brief Returns the path to the XSD Grammar.
 *
 * Return the XSD file with absolute path.
 *
 * \param[out] xsdFile XSD file with absolute path
 * \return bool true if path is found, false else
 * */
bool Directories::xsd(string *xsdFile)
{
  *xsdFile = _xsdDirectory;
  return _xsdFound;
}

bool Directories::checkXsdPath(string xsdPath)
{
  fs::path dir(xsdPath);
  fs::path complete(dir / fs::path(YARS_DIR_ROSIML_NAME));
  return __doesFileExist(complete);
}

bool Directories::library(string *result, string library, string path)
{
  stringstream fullLibraryName;
  fullLibraryName << ROBOT_CONTROLLER_PREFIX << library << DYNAMIC_LIB_SUFFIX;

  fs::path file(fullLibraryName.str());
  if (path.length() > 0)
  {
    fs::path dir(path);
    fs::path complete(dir / file);

    if (__doesFileExist(complete))
    {
      *result = complete.string();
      return true;
    }
  }

  for (unsigned int i = 0; i < _libPathCandidates.size(); i++)
  {
    fs::path searchPath(fs::path(_libPathCandidates[i]) / file);
    if (__doesFileExist(searchPath))
    {
      *result = searchPath.string();
      return true;
    }
  }
  return false;
}

bool Directories::checkLibrariesPath(string path, string library)
{
  fs::path dir(path);
  if (library.length() > 0)
  {
    fs::path complete(dir / fs::path(library));
    return __doesFileExist(complete);
  }
  return __doesDirectoryExist(path);
}

bool Directories::ply(string *result, string ply, string path)
{
  fs::path file(ply);
  if (path.length() > 0)
  {
    fs::path dir(path);
    fs::path complete(dir / file);

    if (__doesFileExist(complete))
    {
      *result = complete.string();
      return true;
    }
  }

  for (unsigned int i = 0; i < _plyPathCandidates.size(); i++)
  {
    fs::path searchPath(fs::path(_plyPathCandidates[i]) / file);
    if (__doesFileExist(searchPath))
    {
      *result = searchPath.string();
      return true;
    }
  }
  return false;
}

bool Directories::checkPlyPath(string path, string file)
{
  fs::path dir(path);
  if (file.length() > 0)
  {
    fs::path complete(dir / fs::path(file));
    return __doesFileExist(complete);
  }
  return __doesDirectoryExist(path);
}

// **************************************************************************
// find functions
// **************************************************************************

/** \brief Check if a directory exists.
 *
 * Requires a path, and checks if the given path exists and is a directory (not
 * a file).
 * \param[in] directoryPath The directory to be checked
 * \return true/false if directory exists/does not exists.
 * */
bool Directories::__doesDirectoryExist(fs::path directoryPath)
{
  if (fs::exists(directoryPath) && fs::is_directory(directoryPath))
    return true;
  else
    return false;
}

/** \brief Check if a file exists.
 *
 * Requires a file name including the path to i, and checks if the given files exists.
 * \param[in] file The file to be checked
 * \return true/false if directory exists/does not exists.
 * */
bool Directories::__doesFileExist(fs::path filePath)
{
  if (fs::exists(filePath) && (!fs::is_directory(filePath)))
  {
    return true;
  }
  return false;
}

/** \brief Returns the first directory that contains a set of files.
 *
 * This function checks a set of directories for the first directory that
 * contains a set of given files. All files must be in the directory. If this is
 * the case, the directory and true is returned, NULL and false otherwise.
 *
 * \param[in] dirs The directories to be checked
 * \param[in] filesnames The set of files to be checked
 * \param[out] returnPath The first directory containing all files
 * \return true/false if a directory exists/does not exists, which contains all
 * files.
 * */
bool Directories::__getFirstExistingDirContainingFiles(std::vector<string>
                                                           dirs,
                                                       std::vector<string> filenames, string *returnPath)
{
  fs::path path;

  for (unsigned int i = 0; i < dirs.size(); i++)
  {
    path = fs::path(dirs[i]);

    if (__doesDirectoryExist(path))
    {
      bool allFilesFound = true;
      for (std::vector<string>::iterator s = filenames.begin(); s != filenames.end(); s++)
      {
        allFilesFound &= __doesFileExist(path / fs::path(*s));
      }

      if (allFilesFound)
      {
        *returnPath = (string)dirs[i];
        return true;
      }
    }
  }
  returnPath = NULL;
  return false;
}

/** \brief Returns the first directory that contains a file.
 *
 * This function checks a set of directories for the first directory that
 * contains a given file. If this is the case, the directory and true is
 * returned, NULL and false otherwise.
 *
 * \param[in] dirs The directories to be checked
 * \param[in] filesname The file to be checked
 * \param[out] returnPath The first directory containing all files
 * \return true/false if a directory exists/does not exists, which contains all
 * files.
 * */
bool Directories::__getFirstExistingDirContainingFile(std::vector<string>
                                                          dirs,
                                                      string *containedFileName, string *returnPath)
{
  fs::path path;
  fs::path file = fs::path(*containedFileName);
  for (unsigned int i = 0; i < dirs.size(); i++)
  {
    path = fs::path(dirs[i]);

    //if(__doesDirectoryExist(path) || file.has_root_dir())
    if (__doesDirectoryExist(path) || (*containedFileName)[0] == '/')
    {
      if (__doesFileExist(path / file))
      {
        *returnPath = (string)dirs[i] + YARS_DIR_DELIMITER + *containedFileName;
        return true;
      }
    }
  }
  returnPath = NULL;
  return false;
}

// **************************************************************************
// Configuration Directories functions
// **************************************************************************

/** \brief Set-up the search directories and their ordering for a
 * configuration file if not specified by the -t command line option.
 *
 * Searches are reduced to a minimum in order to decrease start-up time. Order
 * is:
 * \li ~/.yarsrc
 *
 * in the order given above.
 */
void Directories::__setupConfigDirectories()
{
  stringstream oss;
  string home(getenv("HOME"));
  oss << home << YARS_DIR_DELIMITER << YARS_CONFIG_FILE_NAME;
  _configFilePathCandidate.push_back(oss.str());
#ifndef _MSC_VER
  oss.str("");
  oss << YARS_INSTALL_PATH << YARS_DIR_DELIMITER << YARS_GLOBAL_CONFIG_FILE_NAME;
#endif // _MSC_VER
  _configFilePathCandidate.push_back(oss.str());
  oss.str("");
  oss << YARS_GLOBAL_CONFIG_FILE_DIR << YARS_GLOBAL_CONFIG_FILE_NAME;
  _configFilePathCandidate.push_back(oss.str());
}

bool Directories::configFile(string *file)
{
  for (std::vector<string>::iterator i = _configFilePathCandidate.begin();
       i != _configFilePathCandidate.end(); i++)
  {
    fs::path p(*i);
    if (__doesFileExist(p))
    {
      *file = p.string();
      return true;
    }
  }
  return false;
}

// **************************************************************************
// Xsd Directories functions
// **************************************************************************

/** \brief Set-up the search directory for RoSiML.xsd if not
 * specified by the -x command line option.
 *
 * Searches are reduced to a minimum in order to decrease start-up time. The
 * only directory is:
 * \li CMAKE_INSTALL_PREFIX/share/yars/xsd
 */
void Directories::__setupXsdDirectories()
{
  _xsdPathCandidates.push_back(YARS_DIR_XSD_DIR);
  stringstream oss;
#ifndef _WIN32
  oss << YARS_INSTALL_PATH << YARS_INSTALL_SUFFIX << YARS_DIR_DELIMITER << YARS_DIR_XSD_DIR;
#endif // _WIN32
  _xsdPathCandidates.push_back(oss.str());
}

/** \brief Retrieves directory specified by the -x command line option.
 *
 * Searches are reduced to a minimum in order to decrease start-up time. Only
 * directory is:
 * \li CMAKE_INSTALL_PREFIX/share/yars/xsd
 */
void Directories::__findXsdDirectory()
{
  _xsdFound = false;
  string s = string(YARS_DIR_ROSIML_NAME);

  if (__getFirstExistingDirContainingFile(_xsdPathCandidates, &s, &_xsdDirectory))
  {
    fs::path complete = fs::path(_xsdDirectory);
    if (!complete.has_root_path())
    {
      complete = fs::current_path() / complete;
    }
    _xsdDirectory = complete.string();
    _xsdFound = true;
    return;
  }
}

// **************************************************************************
// Debug functions
// **************************************************************************

/** \brief Builds a string for debug purposes.
 *
 * Returns a string with debug information. Listed are all directories for each
 * class:
 * \li xsd
 * \li textures
 * \li libs
 *
 * \param[in] prefix string used as prefix
 * \param[out] returnString pointer to a string which is filled
 * */
void Directories::toString(string prefix, string *returnString)
{
  *returnString = "";
  for (std::vector<string>::iterator i = _xsdPathCandidates.begin(); i != _xsdPathCandidates.end(); ++i)
  {
    *returnString += prefix + "XSD:      " + *i + "\n";
  }
  for (std::vector<string>::iterator i = _libPathCandidates.begin(); i != _libPathCandidates.end(); ++i)
  {
    *returnString += prefix + "Libs:     " + *i + "\n";
  }
  for (std::vector<string>::iterator i = _plyPathCandidates.begin(); i != _plyPathCandidates.end(); ++i)
  {
    *returnString += prefix + "Plys:     " + *i + "\n";
  }
  for (std::vector<string>::iterator i = _configFilePathCandidate.begin(); i != _configFilePathCandidate.end(); ++i)
  {
    *returnString += prefix + "Configure File: " + *i + "\n";
  }
}

// **************************************************************************
// Libs Directories functions
// **************************************************************************

/** \brief Set-up the search directories and their ordering for controller and
 * object controller libraries if not
 * specified by the -l command line option.
 *
 * Searches are reduced to a minimum in order to decrease start-up time. Order
 * is:
 * \li . (current path)
 * \li ~/.yars/lib
 */
void Directories::__setupLibsDirectories()
{
  string home(getenv("HOME"));
  _libPathCandidates.push_back(YARS_DIR_CURRENT_DIR);
  _libPathCandidates.push_back(YARS_DIR_CURRENT_DIR + YARS_DIR_DELIMITER + YARS_DIR_LIB_DIR);
  _libPathCandidates.push_back(home + YARS_DIR_DELIMITER + YARS_DIR_LOCAL_YARS_DIR + YARS_DIR_DELIMITER + YARS_DIR_LIB_DIR);
  _libPathCandidates.push_back("/usr/local/lib");
}

void Directories::__setupPlyDirectories()
{
  string home(getenv("HOME"));
  _libPathCandidates.push_back(YARS_DIR_CURRENT_DIR);
  _libPathCandidates.push_back(YARS_DIR_CURRENT_DIR + YARS_DIR_DELIMITER + YARS_DIR_PLY_DIR);
  _libPathCandidates.push_back(home + YARS_DIR_DELIMITER + YARS_DIR_LOCAL_YARS_DIR + YARS_DIR_DELIMITER + YARS_DIR_PLY_DIR);
}

/** \brief Returns the directory from which yars was started. Currently only
 * works under Linux/Unix/Mac OS 10.x
 * \param[in]  pointer to std::string
 * \param[in]  argv[0] from command line
 * \param[out] the path to the yars binary, which is currently called
 */
void Directories::getApplicationDirectory(string *s, char *argv)
{
  string pathString = getenv("PATH");
  std::vector<string> path = StringTokeniser::tokenise(pathString, ":");
  for (std::vector<string>::iterator i = path.begin(); i != path.end(); i++)
  {
    fs::path p1(*i);
    fs::path p2(argv);
    fs::path p3(p1 / p2);
    if (__doesFileExist(p3))
    {
      *s = p3.string();
      return;
    }
  }
  *s = "NOT FOUND";
}

void Directories::getFileNameDateTime(string *name)
{
  int index = 1;
  string dateString;
  string dirTmp;
  Timer::getDateTimeString(&dateString);
  stringstream oss;
  oss << *name << "-" << dateString;
  dirTmp = oss.str();
  *name = oss.str();
  while (fs::exists(*name))
  {
    stringstream newOss;
    newOss << dirTmp << "-" << index;
    index++;
    *name = newOss.str();
  }
}

void Directories::getUniqueDirectoryName(string *name)
{
  int index = 1;
  string dateString;
  string dirTmp;
  Timer::getDateString(&dateString);
  stringstream oss;
  oss << *name << "-" << dateString;
  dirTmp = oss.str();
  *name = oss.str();
  while (fs::exists(*name))
  {
    stringstream newOss;
    newOss << dirTmp << "-" << index;
    index++;
    *name = newOss.str();
  }
}

bool Directories::doesFileExist(string file)
{
  fs::path check(file);
  return __doesFileExist(check);
}

void Directories::setFullPath(string *dirString)
{
  fs::path dir(*dirString);
  if (!dir.has_root_path())
  {
    dir = fs::current_path() / dir / YARS_DIR_ROSIML_NAME;
  }
  *dirString = dir.string();
}

bool Directories::getPly(string *result, string filename)
{
  *result = "NONE";
  for (std::vector<string>::iterator i = _plyPathCandidates.begin();
       i != _plyPathCandidates.end(); ++i)
  {
    if (ply(result, filename, *i))
      return true;
  }
  return false;
}

bool Directories::getController(string *result, string filename)
{
  *result = "NONE";
  for (std::vector<string>::iterator i = _libPathCandidates.begin();
       i != _libPathCandidates.end(); ++i)
  {
    if (library(result, filename, *i))
      return true;
  }
  return false;
}

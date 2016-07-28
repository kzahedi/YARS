#ifndef __PLY_LOADER_H__
#define __PLY_LOADER_H__

#include <yars/types/PlyData.h>

#include <string>
#include <map>

using namespace std;

typedef std::map<string, PlyData> Plys;

class PlyLoader
{
  public:
    static PlyLoader* instance();
    PlyData get(string filename);

  private:
    PlyLoader();
    ~PlyLoader();
    PlyData __load(string filename);

    static PlyLoader *_me;

    Plys _data;
};

#endif // __PLY_LOADER_H__

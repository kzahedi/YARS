#include <vector>
#include <string>

using namespace std;

namespace yars {

class StringTokeniser
{
  public:
    static std::vector<string> tokenise(const string& str, const string& delimiters);

};

} // namespace yars

// Temporary global using directive for backward compatibility during namespace transition  
using namespace yars;

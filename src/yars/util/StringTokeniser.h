#include <vector>
#include <string>
#include <string_view>

using namespace std;

namespace yars {

class StringTokeniser
{
  public:
    static std::vector<string> tokenise(string_view str, string_view delimiters);

};

} // namespace yars

// Temporary global using directive for backward compatibility during namespace transition  
using namespace yars;

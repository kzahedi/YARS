#include <vector>
#include <string>
#include <string_view>

using namespace std;

class StringTokeniser
{
  public:
    static std::vector<string> tokenise(string_view str, string_view delimiters);

};

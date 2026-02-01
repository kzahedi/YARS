#include "StringTokeniser.h"

using namespace std;

namespace yars {

// taken from http://www.digitalpeer.com/id/simple
std::vector<string> StringTokeniser::tokenise(string_view str, string_view delimiters)
{
  std::vector<string> tokens;
  string_view::size_type delimPos = 0, tokenPos = 0, pos = 0;

  if(str.length()<1)  return tokens;
  while(1){
    delimPos = str.find_first_of(delimiters, pos);
    tokenPos = str.find_first_not_of(delimiters, pos);

    if(string_view::npos != delimPos){
      if(string_view::npos != tokenPos){
        if(tokenPos<delimPos){
          tokens.emplace_back(str.substr(pos,delimPos-pos));
        }else{
          tokens.emplace_back();
        }
      }else{
        tokens.emplace_back();
      }
      pos = delimPos+1;
    } else {
      if(string_view::npos != tokenPos){
        tokens.emplace_back(str.substr(pos));
      } else {
        tokens.emplace_back();
      }
      break;
    }
  }
  return tokens;
}

} // namespace yars


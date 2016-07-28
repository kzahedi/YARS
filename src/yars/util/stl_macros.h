#ifndef __STL_MACROS_H__
#define __STL_MACROS_H__

# define FOREACH(stl_container, i, container) \
  for(std::vector<stl_container>::iterator i = (container).begin(); i != (container).end(); i++)

# define FOREACHP(stl_container, i, container) \
  for(std::vector<stl_container>::iterator i = (container)->begin(); i != (container)->end(); i++)

# define FOREACHF(stl_container, i, container, begin, end) \
  for(std::vector<stl_container>::iterator i = (container)begin; i != (container)end; i++)

#define STRING_ITERATOR       std::vector<string>::iterator
#define STRING_CONST_ITERATOR std::vector<string>::const_iterator

#endif // __STL_MACROS_H__

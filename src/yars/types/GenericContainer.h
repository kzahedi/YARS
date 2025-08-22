#ifndef __GENERIC_CONTAINER_H__
#define __GENERIC_CONTAINER_H__

#include <vector>
#include <string>
#include <map>
#include <iostream>

//#include <util/RingBuffer.h>

#include "view/console/ConsoleView.h"

template<typename _KEY, typename _VALUE,
  typename _CONT = std::map<_KEY,_VALUE> >
class GenericContainer
{
  private:
    _CONT map_;

  public:
    GenericContainer();
    ~GenericContainer();

    using iterator               = typename _CONT::iterator;
    using const_iterator         = typename _CONT::const_iterator;
    using size_type              = typename _CONT::size_type;
    using difference_type        = typename _CONT::difference_type;
    using reverse_iterator       = typename _CONT::reverse_iterator;
    using const_reverse_iterator = typename _CONT::const_reverse_iterator;

    int getSize();
    // TODO: rename getSize to size
    int size()
    {
      return getSize();
    }
    int getMaxSize();

    bool isEmpty();
    int  count(const _KEY&);

    void clear();
    int  erase(const _KEY&);

    bool doesKeyExist(const _KEY& key);

    _VALUE&                   operator[](const _KEY&);
    _VALUE&                   getValue(const _KEY&);
    void                      setValue(const _KEY&, const _VALUE);
    const std::vector<_VALUE> getValueRange(const _KEY&);

    bool insertPair(const _KEY&, const _VALUE&);

    iterator begin()
    {
      return map_.begin();
    }

    const_iterator begin() const
    {
      return map_.begin();
    }

    iterator end()
    {
      return map_.end();
    }

    const_iterator end() const
    {
      return map_.end();
    }

    reverse_iterator rbegin()
    {
      return map_.rbegin();
    }

    const_reverse_iterator rbegin() const
    {
      return map_.rbegin();
    }

    reverse_iterator rend()
    {
      return map_.rend();
    }

    const_reverse_iterator rend() const
    {
      return map_.rend();
    }
};


template<typename _KEY, typename _VALUE, typename _CONT>
GenericContainer<_KEY,_VALUE,_CONT>::GenericContainer()
{
  // nothing to do
}

template<typename _KEY, typename _VALUE, typename _CONT>
GenericContainer<_KEY,_VALUE,_CONT>::~GenericContainer()
{
  // nothing to do
}

template<typename _KEY, typename _VALUE, typename _CONT>
int GenericContainer<_KEY,_VALUE,_CONT>::getSize()
{
  return map_.size();
}

template<typename _KEY, typename _VALUE, typename _CONT>
int GenericContainer<_KEY,_VALUE,_CONT>::getMaxSize()
{
  return map_.max_size();
}

template<typename _KEY, typename _VALUE, typename _CONT>
bool GenericContainer<_KEY,_VALUE,_CONT>::isEmpty()
{
  return map_.empty();
}

template<typename _KEY, typename _VALUE, typename _CONT>
int GenericContainer<_KEY,_VALUE,_CONT>::count(const _KEY& key)
{
  return map_.count(key);
}

template<typename _KEY, typename _VALUE, typename _CONT>
void GenericContainer<_KEY,_VALUE,_CONT>::clear()
{
  map_.clear();
}

template<typename _KEY, typename _VALUE, typename _CONT>
int GenericContainer<_KEY,_VALUE,_CONT>::erase(const _KEY& key)
{
  return map_.erase(key);
}

template<typename _KEY, typename _VALUE, typename _CONT>
bool GenericContainer<_KEY,_VALUE,_CONT>::doesKeyExist(const _KEY& key)
{
  typename _CONT::iterator iter;
  iter = map_.find(key);

  if( iter != map_.end() )
  {
    return true;
  }
  else
  {
    return false;
  }
}

template<typename _KEY, typename _VALUE, typename _CONT>
_VALUE& GenericContainer<_KEY,_VALUE,_CONT>::operator[](const _KEY& key)
{
  return map_[key];
}

template<typename _KEY, typename _VALUE, typename _CONT>
_VALUE& GenericContainer<_KEY,_VALUE,_CONT>::getValue(const _KEY& key)
{
  typename _CONT::iterator iter;
  iter = map_.find(key);

  if( iter != map_.end() )
  {
    return iter->second;
  }
  else
  {
    throw std::string("GenericContainer<_KEY,_VALUE,_CONT>::getValue(): key not found");
  }
}

template<typename _KEY, typename _VALUE, typename _CONT>
void GenericContainer<_KEY,_VALUE,_CONT>::setValue(const _KEY& key, const _VALUE value)
{
  typename _CONT::iterator iter;
  iter = map_.find(key);

  if( iter != map_.end() )
  {
    iter->second = value;
  }
  else
  {
    throw std::string("GenericContainer<_KEY,_VALUE,_CONT>::setValue(): key not found");
  }
}
template<typename _KEY, typename _VALUE, typename _CONT>
const std::vector<_VALUE> GenericContainer<_KEY,_VALUE,_CONT>::getValueRange(
    const _KEY& key)
{
  std::pair<typename _CONT::iterator, typename _CONT::iterator> p;

  typename _CONT::iterator iter;

  std::vector<_VALUE> rangeValueVector;

  p = map_.equal_range(key);

  if ( (p.first == map_.end( )) && (p.second == map_.end( )) )
  {
    throw std::string("GenericContainer<_KEY,_VALUE,_CONT>::getValueRange(): \
        no key less or equal than requested key");
  }
  else
  {
    // TODO: recheck this part
    for(iter = p.first; iter != p.second; ++iter)
    {
      rangeValueVector.push_back((*iter).second);
    }
    return rangeValueVector;
  }
}

template<typename _KEY, typename _VALUE, typename _CONT>
bool GenericContainer<_KEY,_VALUE,_CONT>::insertPair(
    const _KEY& key, const _VALUE& value)
{
  std::pair<typename _CONT::iterator, bool> p;

  p = map_.insert(std::make_pair(key, value));

  if(p.second == false)
  {
    Y_WARN("GenericContainer<_KEY, _VALUE, _CONT>::insertPair failed %s\n");
    cout << "key: \""<< key << "\"" << endl;
  }

  return p.second;
}

#endif

#ifndef util__h
#define util__h

//#include "signalRouter.h"

#include <algorithm>
#include <vector>
#include <map>

namespace util {
template <class T>
auto findMedian(T in){
  int size = in.size();
  std::sort (in.begin(), in.end());
  if(size % 2){   //odd
    return in[ ( size - 1 ) / 2 ];
  }else{          //even
    return ( in[ size / 2 ] + in[ ( size / 2 ) - 1 ] ) / 2;
  }
}

template <typename T>
void moveToBorders(T &value, T const& min, T const& max)
{
  if(value < min){
    value = min;
  }else if(value > max){
    value = max;
  }
}

template <typename T>
void moveToBorders(
  T &value, T const& min, T const& max, T const& min_value, T const& max_value)
{
  if(value < min){
    value = min_value;
  }else if(value > max){
    value = max_value;
  }
}

/*
Generic function to find an element in vector and also its position.
It returns a pair of bool & int i.e.

bool : Represents if element is present in vector or not.
int : Represents the index of element in vector if its found else -1

*/
template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element)
{
    std::pair<bool, int > result;

    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

    if (it != vecOfElements.end())
    {
        result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }
    return result;
}

template < typename T>
bool eraseSingleElementInVector(std::vector<T>  & vecOfElements, const T  & element)
{
    std::pair<bool, int> searchResult = util::findInVector(vecOfElements, element);
    if(searchResult.first){
        vecOfElements.erase(vecOfElements.begin() + searchResult.second);
        return true;
    }else{
        return false;
    }
}
template <typename T1, typename T2>
bool searchInMap(std::map<T1, T2> & _map, const T1 & compareElement, T2 *& foundElement)
{
    typename  std::map<T1,T2>::iterator it = _map.find(compareElement);
    if(it != _map.end()){
        foundElement = &(it->second);
        return true;
    }else{
        return false;
    }
}
}




#endif

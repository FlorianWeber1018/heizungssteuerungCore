#ifndef util__h
#define util__h

//#include "signalRouter.h"

#include <algorithm>
#include <vector>



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



#endif

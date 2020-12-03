#include "header.h"

double getTime(){
  gettimeofday(&curTime,NULL);

  double second = curTime.tv_sec;
  double milSecond = (double) curTime.tv_usec / 1000000;
  double res = second + milSecond;
  return res;
}

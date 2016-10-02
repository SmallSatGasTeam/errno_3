#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

template <typename T>
class MedianFilter {
public:
  // MedianFilter(){ //TODO remove
  //   timeCounter = 0;
  //   buffLen = 0;
  //   timeBuff = (T*)0;
  //   valBuff = (T*)0;
  // }
  MedianFilter(T* timeBuff, T* valBuff, int buffLen)
    :timeBuff(timeBuff), valBuff(valBuff), buffLen(buffLen){
      timeCounter = 0;
    }

  void addDataPoint(T val){
    int toReplace = timeBuff[timeCounter];
    int timePosition = findInBuff(timeBuff, buffLen, toReplace);
    int valPosition = findInBuff(valBuff, buffLen, toReplace);

    if(timePosition >= 0 && timePosition < buffLen)
      timeBuff[timePosition] = val;
    if(valPosition >= 0 &&  valPosition < buffLen){
      valBuff[valPosition] = val;
      sortBuffer(valBuff, buffLen);
    }

    timeCounter = incrementCount(timeCounter, buffLen);
  }

  T getFilteredDataPoint(){
    sortBuffer(valBuff, buffLen);
    int midPoint = buffLen / 2;
    return valBuff[midPoint];
  }

  void sortBuffer(T* buff, int buffLen){
    for(int i = 0; i < buffLen; i++){
      T val = buff[i];
      for(int j = i - 1; j >= 0; j--){
        if(val < buff[j]){
          buffSwap(buff, i, j);
          break;
        }
      }
    }
  }

  void buffSwap(T* buff, int i, int j){
    T temp = buff[i];
    buff[i] = buff[j];
    buff[j] = temp;
  }

  int inline incrementCount(int timeCounter, int buffLen){
    return ++timeCounter %= buffLen;
  }

  int findInBuff(T* buff,int buffLen, T find){
    for(int i = 0; i < buffLen; i++){
      if(buff[i] == find) return i;
    }
    return -1;
  }

  int timeCounter;
  int buffLen;
  T* timeBuff;
  T* valBuff;
};
#endif

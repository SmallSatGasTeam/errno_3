#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

template <typename T>
class DataFilter {
  DataFilter(T* timeBuff, T* valBuff, int buffLen)
    :timeBuff(timeBuff), valBuff(valBuff), buffLen(buffLen){
      timeCounter = 0;
    }

  void addDataPoint(){
    int toReplace = timeBuff[timeCounter];
    int timePosition = findInBuff(timeBuff, buffLen, toReplace);
    int valPosition = findInBuff(valBuff, buffLen, toReplace);

    replaceAtIndex(timeBuff, timePosition, toReplace);
    replaceAtIndex(valBuff, valPosition, toReplace);

    incrementCount();
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
        if(val > buff[j]){
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

  int inline incrementCount(){
    ++timeCounter %= buffLen;
  }

  int findInBuff(T* buff,int buffLen, T find){
    for(int i = 0; i < buffLen; i++){
      if(buff[i] == find) return i;
    }
    return -1;
  }

  bool replaceAtIndex(T* buff, int index, T replaceVal){
    if(index < 0) return false;
    buff[index] = replaceVal;
    return true;
  }

  int timeCounter;
  int buffLen;
  T* timeBuff;
  T* valBuff;
};
#endif

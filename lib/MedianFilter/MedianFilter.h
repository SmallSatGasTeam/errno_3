#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

template <typename T>
class MedianFilter {
public:
  MedianFilter(T* timeBuff, T* valBuff, int buffLen)
    :timeBuff(timeBuff), valBuff(valBuff), buffLen(buffLen){
      timeCounter = 0;
      tempBuffLen = 0;
    }

  int getCurrentLength(){
    return tempBuffLen > buffLen ? buffLen : tempBuffLen;
  }

  void addDataPoint(T val){
    tempBuffLen++;
    int currentLength = getCurrentLength();

    if(tempBuffLen < buffLen){
      // Handle when buffer is not yet full
      timeBuff[currentLength-1] = val;
      valBuff[currentLength-1] = val;
    } else {
      // If our buffer is full we need to replace values
      int toReplace = timeBuff[timeCounter];
      int timePosition = findInBuff(timeBuff, currentLength, toReplace);
      int valPosition = findInBuff(valBuff, currentLength, toReplace);

      if(timePosition >= 0 && timePosition < currentLength)
        timeBuff[timePosition] = val;
      if(valPosition >= 0 &&  valPosition < currentLength){
        valBuff[valPosition] = val;
      }
    }

    sortBuffer(valBuff, currentLength);
    timeCounter = incrementCount(timeCounter, buffLen);
  }

  T getFilteredDataPoint(){
    int currentLength = getCurrentLength();
    sortBuffer(valBuff, currentLength);
    int midPoint = currentLength / 2;
    return valBuff[midPoint];
  }

  void sortBuffer(T* buff, int buffLen){
    for(int i = 1; i < buffLen; i++){
      for(int j = i - 1; j >= 0; j--){
        if(buff[j+1] < buff[j]){
          buffSwap(buff, j+1, j);
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

  protected:
  int timeCounter;
  int tempBuffLen;
  int buffLen;
  T* timeBuff;
  T* valBuff;
};
#endif

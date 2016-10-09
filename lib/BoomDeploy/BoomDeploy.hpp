extern const float DEPLOY_MIN_PRESSURE;
extern const float DEPLOY_MAX_PRESSURE;
//extern class Stream; //Uncomment to run local tests

bool shouldDeployBoom(bool deployed, bool initialized, bool confirmed, float pressure){
  if(initialized == true && confirmed == true){ return true; }
  if(deployed == true) { return false; }
  if(pressure <= DEPLOY_MAX_PRESSURE && pressure >= DEPLOY_MIN_PRESSURE){ return true; }

  return false;
}

char getMessage(Stream** input_streams){
  char received_message = 0;
    // We don't expect to receive commands from two streams at the same time. So this
    // overwriting of the message shouldn't be a problem.

  for (char i = 0; input_streams[i] != 0; ++i)
  {
    if (input_streams[i]->available())
    {
      received_message = input_streams[i]->read();
      while (input_streams[i]->available())
      {
        input_streams[i]->read();
      }
      // Clear the rest of the buffer
    }
  }
  return received_message;
}

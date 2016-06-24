/* InputHandler.h
 * 
 * 
 * Corey Shuman
 * 1/22/16
 */
#ifndef InputHandler_h
#define InputHandler_h
#include "Arduino.h"
#include <WString.h>

const int MAX_ARG = 20;

struct HandlerDef {
  String name;
  int maxArgument;
  void (*func)(String[], int);
};

class InputHandler
{
  public:
    InputHandler(int handlerCount, int tmo);
    int AddHandler(String name, int maxArgument, void (*func)(String*, int));
    void Reset();
    void Proc();
    void Init(HardwareSerial *ser);
    
  private:
    void ExecuteHandler(int handlerIdx);
    int FindHandler(String name);
    int handlerCount;
    int handlerCountMax;
    unsigned long timeout;
    int timeoutInterval;
    int state;
    int index;
    String command;
    String arguments[MAX_ARG];
    int argumentCount;
    HandlerDef handlerArr[10];
    HardwareSerial *serial;
  
};

#endif

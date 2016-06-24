/* InputHandler.cpp
 * 
 * 
 * Corey Shuman
 * 1/22/16
 */
#include "Arduino.h"
#include "InputHandler.h"

const char DELIM1 = ' ';
const char DELIM2 = ',';
const char ENDL = '\n';



/* Constructor */
InputHandler::InputHandler(int handlerCntMax, int tmo)
{
  handlerCountMax = handlerCntMax;
  timeoutInterval = tmo;
  //handlerArr = new HandlerDef[handlerCount];
  handlerCount = 0;
}

void InputHandler::Init(HardwareSerial *ser)
{
  serial = ser;
  Reset();
}

/* Call periodically to run the handler */
void InputHandler::Proc()
{
  char inByte;

  if(timeoutInterval > 0)
  {
    if((state > 0 || index > 0) && (millis() - timeout > timeoutInterval))
    {
      Reset();
    }
  }
  
  while(serial->available() > 0)
  {
    timeout = millis();
    inByte = serial->read();
    if(inByte == DELIM1 || inByte == DELIM2)
    {
      state ++;
      continue;
    }
    else if(inByte == ENDL)
    {
      argumentCount = state;
      if(argumentCount > MAX_ARG)
        argumentCount = MAX_ARG;
      // cts debug
      //serial->print("search...\n");
      int handle = FindHandler(command);
      if(handle > -1) {
        // cts debug
        //serial->print("found\n");
        ExecuteHandler(handle);
      }
      Reset();
      continue;
    }
    if(state == 0)
    {
      command.concat(inByte);
      // cts debug
      //serial->print(command);
    }
    else if (state >= 1 && state <= MAX_ARG)
    {
      arguments[state-1].concat(inByte);
      // cts debug
      //serial->print(arguments[state-1]);
    }
  }
}

void InputHandler::Reset()
{
  state = 0;
  index = 0;
  command = "";
  for(int i = 0; i < MAX_ARG; i++)
  {
    arguments[i] = "";
  }
}

/* Add a command handler function to handler pool */
int InputHandler::AddHandler(String name, int maxArgument, void (*func)(String*, int))
{
  if(handlerCount >= handlerCountMax)
  {
    serial->println("Err: Max Handler");
    return -1;
  }
  handlerArr[handlerCount].name = String(name);
  //serial->print("-A>");
  //serial->print(handlerArr[handlerCount].name);
  //serial->print("\n");
  handlerArr[handlerCount].maxArgument = maxArgument;
  handlerArr[handlerCount].func = func;
  return handlerCount++;
}

/* Called to run a located handler */
void InputHandler::ExecuteHandler(int handlerIdx)
{
  if(handlerIdx >= 0 && handlerIdx < handlerCount)
    handlerArr[handlerIdx].func(arguments, argumentCount);
}

/* Used to match a command to a handler */
int InputHandler::FindHandler(String name)
{
  int i;
  int ret = -1;
  //serial->print("-->");
  //serial->print(name);
  //serial->print("--\n");
  for(i = 0; i < handlerCount; i++)
  {
    HandlerDef *thisHandler = &handlerArr[i];
    //serial->print("--");
    //serial->print(handlerArr[i].name);
    //serial->print("--\n");
    if(thisHandler->name.compareTo(name) == 0)
    {
      ret = i;
      break;
    }
  }
  return ret;
}


/* PixelLED.h
 * 
 * 
 * Corey Shuman
 * 1/28/16
 */

#ifndef PixelLED_h
#define PixelLED_h

#include <Adafruit_NeoPixel.h>

class PixelLED
{
  public:
    PixelLED();
    void Init(int pin, int count);
    void Proc();
    void SetPixel(int r, int g, int b);

  private:
    unsigned long tick;
    int state;
    Adafruit_NeoPixel* pixels;

  
};


#endif

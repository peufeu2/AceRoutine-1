/*
MIT License

Copyright (c) 2021 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ACE_ROUTINE_CLOCK_INTERFACE_H
#define ACE_ROUTINE_CLOCK_INTERFACE_H

#include <stdint.h>
#include <Arduino.h>
#include <AceCommon.h> // udiv1000()

namespace ace_routine {

/**
 * A utility class (all methods are static) that provides a layer of indirection
 * to Arduino clock functions (millis() and micros()). This thin layer of
 * indirection allows injection of a different ClockInterface for testing
 * purposes. For normal code, the compiler will optimize away the extra function
 * call.
 */
class ClockInterface {
  public:
    /** Get the current micros. */
    static unsigned long micros() { return ::micros(); }

    /** Get the current millis. */
    static unsigned long millis() { return ::millis(); }

    /**
     * Get the current seconds. When the 32-bit 'unsigned long' rolls over at
     * 4294967296 millis, this function will return the next integer second too
     * early, more precisely, 0.704 seconds too early. Fortunately, this happens
     * only once every 4294967 seconds, i.e. 49.7 days. For the purposes of
     * COROUTINE_DELAY_SECONDS(), I think this is good enough.
     */
    static unsigned long seconds() {
      unsigned long m = ::millis();
    #if defined(ARDUINO_ARCH_AVR) \
        || defined(ARDUINO_ARCH_SAMD) \
        || defined(ESP8266)
      // No hardware division so the udiv1000() approximation is faster
      return ace_common::udiv1000(m);
    #else
      return m / 1000;
    #endif
    }
};

}

#endif

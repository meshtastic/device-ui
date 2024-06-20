#pragma once

#include <stdint.h>

class LinuxHelper
{
  public:
    static uint32_t getAvailableMem(void);
    static uint32_t getFreeMem(void);
    static uint32_t getTotalMem(void);

  protected:
    static uint32_t getMem(const char *entry);
};
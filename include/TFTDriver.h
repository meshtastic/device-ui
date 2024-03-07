#pragma once

#include "DisplayDriver.h"

template <class TFT> class TFTDriver : public DisplayDriver {
public:
  TFTDriver(TFT *tft, uint16_t width, uint16_t height);
  virtual void init(void);

protected:
  TFT *tft;
};

template <class TFT>
TFTDriver<TFT>::TFTDriver(TFT *tft, uint16_t width, uint16_t height)
    : DisplayDriver(width, height), tft(tft) {}

template <class TFT> void TFTDriver<TFT>::init(void) { DisplayDriver::init(); }

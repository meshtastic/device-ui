#include "graphics/view/OLED/OLEDViewController.h"

OLEDViewController::OLEDViewController() {}

void OLEDViewController::init(MeshtasticView *gui, IClientBase *_client)
{
    ViewController::init(gui, _client);
}

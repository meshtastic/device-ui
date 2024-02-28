#include "ViewController.h"


ViewController::ViewController() : client(nullptr) {

}


void ViewController::init(IClientBase* _client) {
    client = _client;
}


void ViewController::runOnce(void) {

}

ViewController::~ViewController() {

}


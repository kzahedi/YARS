#include "NIL.h"

using namespace std;

void Nil::update(){
}

void Nil::reset(){
  std::cout<<"Nil-Controller reset\n";
}

void Nil::init(){
  std::cout<<"Nil-Controller initialised\n";
}

void Nil::close(){
  std::cout<<"Nil-Controller closed\n";
}


// the class factories

extern "C" RobotController* create() {
  return new Nil;
}

extern "C" void destroy(RobotController* controller) {
  // delete controller;
}


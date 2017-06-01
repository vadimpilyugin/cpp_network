#include "functions2.h"
#include "network.h"

int main(int argc, char *argv[]) {
  func2();
  throw Network::Exception("I don't know");
}

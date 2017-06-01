#include "printer.h"
#include "functions1.h"
#include "functions2.h"

void func2() {
  Printer::debug("Entered function 2");
  func1();
  Printer::debug("Exiting from function 2");
}

#include <iostream>
#include <string>
#include <memory>

#include "proxy.h"

struct Streamable : pro::facade_builder
    ::add_convention<pro::operator_dispatch<"<<", true>, std::ostream&(std::ostream& out) const>
    ::build {};

int main() {
  std::string str = "Hello World";
  pro::proxy<Streamable> p1 = &str;
  std::cout << "p1 = " << *p1 << "\n";  // Prints: "p1 = Hello World"

  pro::proxy<Streamable> p2 = std::make_unique<int>(123);
  std::cout << "p2 = " << *p2 << "\n";  // Prints: "p2 = 123"

  pro::proxy<Streamable> p3 = pro::make_proxy<Streamable>(3.14);
  std::cout << "p3 = " << *p3 << "\n";  // Prints: "p3 = 3.14"
}

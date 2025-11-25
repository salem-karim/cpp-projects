#include "Spiel/Spiel.hpp"
#include <memory>

int main() {
  std::unique_ptr<Spiel> spiel = std::make_unique<Spiel>();
  spiel->spielStart();
  spiel->spielEnde();
  return 0;
}

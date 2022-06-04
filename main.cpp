#include "HelloTriangleApplication.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
  HelloTriangleApplication app;

  try {
    app.Run();
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
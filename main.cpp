#include "HelloTriangleApplication.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
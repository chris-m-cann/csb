#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    auto return_v = Catch::Session().run(argc, argv);
    std::cin.get();
    return return_v;
}

#include <iostream>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

using json = nlohmann::json;

int main(int, char **)
{
    std::cout << "Hello, world!\n";

    json v = {"key", "value"};

    fmt
}

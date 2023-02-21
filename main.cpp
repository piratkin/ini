#include <iostream>
#include <filesystem>
#include"ini.h"

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Invalid argument!"
            << std::endl;
        return 1;
    }

    std::filesystem::path path(argv[1]);
    auto file = std::filesystem::absolute(path);

    // Open INI-file
    ini::File ini(file.string());
    if (ini.isOpen()) {
        std::cout << "opened: ";
    } else {
        std::cout << "closed: ";
    }
    std::cout << file.string() << std::endl;

    std::cout << "errors: " << ini.ok()
         << std::endl;

    // Print dump
    std::cout << ini.dump().value_or("-") << std::endl;

    // Save as new INI-file
    ini.save(file.generic_string() + ".bak");

    return 0;
}

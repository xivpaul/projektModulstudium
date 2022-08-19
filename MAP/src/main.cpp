
#include <iostream>

#include "CSV.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {

  Server::getInstance()->start();

  // std::cout << getMetadata();
}


#include <iostream>

#include "Server.hpp"

int main(int argc, char **argv) {

  Server::getInstance()->start();

  // std::cout << getMetadata();
}

#pragma once

#include "Command.h"
#include <iostream>

class EchoCommand : public Command {
public:
  std::string getName() const override { return "echo"; }

  int execute(const std::vector<std::string> &args) override {
    for (size_t i = 1; i < args.size(); ++i) {
      if (i > 1)
        std::cout << " ";
      std::cout << args[i];
    }
    std::cout << std::endl;
    return 0;
  }
};

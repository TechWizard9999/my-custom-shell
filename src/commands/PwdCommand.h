#pragma once

#include "Command.h"
#include <iostream>
#include <limits.h>
#include <unistd.h>

class PwdCommand : public Command {
public:
  std::string getName() const override { return "pwd"; }

  int execute(const std::vector<std::string> &args) override {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
      std::cout << cwd << std::endl;
      return 0;
    } else {
      std::cerr << "pwd: error getting current directory" << std::endl;
      return 1;
    }
  }
};


#pragma once

#include "../utils/PathSearcher.h"
#include "Command.h"
#include <iostream>
#include <string>
#include <vector>

class Shell;

class TypeCommand : public Command {
private:
  Shell &shell;

public:
  TypeCommand(Shell &s) : shell(s) {}

  std::string getName() const override { return "type"; }

  int execute(const std::vector<std::string> &args) override {
    if (args.size() < 2)
      return 1;

    std::string target = args[1];

    if (shell.hasCommand(target)) {
      std::cout << target << " is a shell builtin" << std::endl;
      return 0;
    }

    std::string fullPath = PathSearcher::find(target);
    if (!fullPath.empty()) {
      std::cout << target << " is " << fullPath << std::endl;
      return 0;
    }

    std::cout << target << ": not found" << std::endl;
    return 0;
  }
};

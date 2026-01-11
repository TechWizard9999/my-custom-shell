#pragma once

#include "Command.h"
#include <cstdlib>

class ExitCommand : public Command {
public:
  std::string getName() const override { return "exit"; }

  int execute(const std::vector<std::string> &args) override {
    std::exit(0);
    return 0;
  }
};

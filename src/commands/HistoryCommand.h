#pragma once

#include "../Shell.h"
#include "Command.h"
#include <iostream>
#include <string>
#include <vector>

    // class Shell; // No longer needed

class HistoryCommand : public Command {
private:
  Shell &shell;

public:
  HistoryCommand(Shell &s) : shell(s) {}

  std::string getName() const override { return "history"; }

  int execute(const std::vector<std::string> &args) override {
    const auto &history = shell.getHistory();
    for (size_t i = 0; i < history.size(); ++i) {
      std::cout << "    " << (i + 1) << "  " << history[i] << std::endl;
    }
    return 0;
  }
};

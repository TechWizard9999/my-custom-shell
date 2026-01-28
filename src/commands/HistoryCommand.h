#pragma once

#include "../Shell.h"
#include "Command.h"
#include <iostream>
#include <string>
#include <vector>

class HistoryCommand : public Command {
private:
  Shell &shell;

public:
  HistoryCommand(Shell &s) : shell(s) {}

  std::string getName() const override { return "history"; }

  int execute(const std::vector<std::string> &args) override {
    const auto &history = shell.getHistory();
    size_t startIndex = 0;

    if (args.size() > 1) {
      try {
        int n = std::stoi(args[1]);
        if (n < 0) {

        } else if (static_cast<size_t>(n) < history.size()) {
          startIndex = history.size() - n;
        }
      } catch (...) {
        std::cout << "history: numeric argument required" << std::endl;
        return 1;
      }
    }

    for (size_t i = startIndex; i < history.size(); ++i) {
      std::cout << "    " << (i + 1) << "  " << history[i] << std::endl;
    }
    return 0;
  }
};

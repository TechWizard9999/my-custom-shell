#pragma once

#include "../Shell.h"
#include "Command.h"
#include <fstream>
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
    if (args.size() > 1 && args[1] == "-r") {
      if (args.size() < 3) {
        std::cerr << "history: option requires an argument" << std::endl;
        return 1;
      }
      std::ifstream historyFile(args[2]);
      if (!historyFile.is_open()) {
        std::cerr << "history: " << args[2] << ": No such file or directory"
                  << std::endl;
        return 1;
      }
      std::string line;
      while (std::getline(historyFile, line)) {
        if (!line.empty()) {
          shell.appendToHistory(line);
        }
      }
      return 0;
    }

    if (args.size() > 1 && args[1] == "-w") {
      if (args.size() < 3) {
        std::cerr << "history: option requires an argument" << std::endl;
        return 1;
      }
      std::ofstream historyFile(args[2]);
      if (!historyFile.is_open()) {
        std::cerr << "history: " << args[2] << ": No such file or directory"
                  << std::endl;
        return 1;
      }
      const auto &history = shell.getHistory();
      for (const auto &cmd : history) {
        historyFile << cmd << std::endl;
      }
      return 0;
    }

    if (args.size() > 1 && args[1] == "-a") {
      if (args.size() < 3) {
        std::cerr << "history: option requires an argument" << std::endl;
        return 1;
      }
      std::ofstream historyFile(args[2], std::ios::app);
      if (!historyFile.is_open()) {
        std::cerr << "history: " << args[2] << ": No such file or directory"
                  << std::endl;
        return 1;
      }
      const auto &history = shell.getHistory();
      for (const auto &cmd : history) {
        historyFile << cmd << std::endl;
      }
      return 0;
    }

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

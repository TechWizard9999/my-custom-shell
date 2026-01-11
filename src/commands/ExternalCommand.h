#pragma once

#include "../utils/PathSearcher.h"
#include "Command.h"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class ExternalCommand : public Command {
public:
  std::string getName() const override { return ""; }

  int execute(const std::vector<std::string> &args) override {
    std::string cmdName = args[0];
    std::string fullPath = PathSearcher::find(cmdName);

    if (fullPath.empty()) {
      std::cout << cmdName << ": not found" << std::endl;
      return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
      std::vector<char *> exec_args;
      for (const auto &arg : args) {
        exec_args.push_back(const_cast<char *>(arg.c_str()));
      }
      exec_args.push_back(nullptr);
      execv(fullPath.c_str(), exec_args.data());
      perror("execv failed");
      exit(1);
    } else if (pid > 0) {
      int status;
      waitpid(pid, &status, 0);
      return WEXITSTATUS(status);
    } else {
      std::cerr << "Fork failed" << std::endl;
      return 1;
    }
  }
};

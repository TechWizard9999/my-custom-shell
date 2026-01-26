#pragma once

#include <cstdlib>
#include <functional>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class PipelineExecutor {
public:
  using CommandFunc = std::function<int(const std::vector<std::string> &)>;

  static void execute(const std::vector<std::vector<std::string>> &cmds,
                      CommandFunc execFunc) {
    int numCmds = cmds.size();
    if (numCmds == 0)
      return;

    if (numCmds == 1) {
      execFunc(cmds[0]);
      return;
    }

    std::vector<int> pipes((numCmds - 1) * 2);
    for (int i = 0; i < numCmds - 1; i++) {
      if (pipe(&pipes[i * 2]) < 0) {
        perror("pipe");
        return;
      }
    }

    std::vector<pid_t> pids;
    for (int i = 0; i < numCmds; i++) {
      pid_t pid = fork();
      if (pid == 0) {
        if (i > 0) {
          dup2(pipes[(i - 1) * 2], STDIN_FILENO);
        }
        if (i < numCmds - 1) {
          dup2(pipes[i * 2 + 1], STDOUT_FILENO);
        }
        for (size_t j = 0; j < pipes.size(); j++) {
          close(pipes[j]);
        }
        exit(execFunc(cmds[i]));
      }
      pids.push_back(pid);
    }

    for (size_t i = 0; i < pipes.size(); i++) {
      close(pipes[i]);
    }

    for (pid_t pid : pids) {
      waitpid(pid, nullptr, 0);
    }
  }
};

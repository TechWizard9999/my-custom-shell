#pragma once

#include "commands/Command.h"
#include "commands/ExternalCommand.h"
#include "parsers/CommandParser.h"
#include "utils/Autocompleter.h"
#include "utils/IORedirector.h"
#include "utils/PathSearcher.h"
#include <iostream>
#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <unordered_map>
#include <vector>

class Shell {
private:
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;

public:
  void registerCommands(Command *cmd) {
    commands[cmd->getName()] = std::unique_ptr<Command>(cmd);
  }

  bool hasCommand(const std::string &name) const {
    return commands.find(name) != commands.end();
  }

  int executeCommand(const std::vector<std::string> &args) {
    if (args.empty())
      return 0;
    std::string cmdName = args[0];
    if (hasCommand(cmdName)) {
      return commands[cmdName]->execute(args);
    } else {
      static ExternalCommand externalCmd;
      return externalCmd.execute(args);
    }
  }

  void executePipeline(const std::vector<std::vector<std::string>> &cmds) {
    int numCmds = cmds.size();
    if (numCmds == 0)
      return;

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
        exit(executeCommand(cmds[i]));
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

  void run() {
    Autocomplete::initialize();

    while (true) {
      char *input_c = readline("$ ");

      if (!input_c) {
        break;
      }

      std::string input(input_c);
      if (input.length() > 0) {
        add_history(input_c);
      }
      free(input_c);

      if (input.empty())
        continue;

      std::pair<std::vector<std::string>, Redirection> parsed =
          CommandParser::parseWithRedirection(input);
      std::vector<std::string> args = parsed.first;
      Redirection redir = parsed.second;

      if (args.empty())
        continue;

      std::vector<std::vector<std::string>> pipelineCmds;
      std::vector<std::string> currentCmd;

      for (const auto &arg : args) {
        if (arg == "|") {
          if (!currentCmd.empty()) {
            pipelineCmds.push_back(currentCmd);
            currentCmd.clear();
          }
        } else {
          currentCmd.push_back(arg);
        }
      }
      if (!currentCmd.empty()) {
        pipelineCmds.push_back(currentCmd);
      }

      if (pipelineCmds.size() > 1) {
        executePipeline(pipelineCmds);
        continue;
      }

      IORedirector ioRedirector;
      if (redir.hasOutputRedirect) {
        ioRedirector.redirectOutput(redir.outputFile, redir.appendOutput);
      }
      if (redir.hasErrorRedirect) {
        ioRedirector.redirectError(redir.errorFile, redir.appendError);
      }

      executeCommand(args);

      ioRedirector.restore();
    }
  }
};

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

  void executePipeline(const std::vector<std::string> &leftArgs,
                       const std::vector<std::string> &rightArgs) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
      perror("pipe");
      return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
      close(pipefd[0]);
      dup2(pipefd[1], STDOUT_FILENO);
      close(pipefd[1]);
      exit(executeCommand(leftArgs));
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
      close(pipefd[1]);
      dup2(pipefd[0], STDIN_FILENO);
      close(pipefd[0]);
      exit(executeCommand(rightArgs));
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
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

      std::vector<std::string> leftCmd;
      std::vector<std::string> rightCmd;
      bool hasPipe = false;

      for (const auto &arg : args) {
        if (arg == "|") {
          hasPipe = true;
          continue;
        }
        if (hasPipe) {
          rightCmd.push_back(arg);
        } else {
          leftCmd.push_back(arg);
        }
      }

      if (hasPipe) {
        if (!leftCmd.empty() && !rightCmd.empty()) {
          executePipeline(leftCmd, rightCmd);
        }
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

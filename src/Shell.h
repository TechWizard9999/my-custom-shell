#pragma once

#include "commands/Command.h"
#include "commands/ExternalCommand.h"
#include "parsers/CommandParser.h"
#include "utils/Autocompleter.h"
#include "utils/IORedirector.h"
#include "utils/PathSearcher.h"
#include "utils/PipelineExecutor.h"
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

  std::vector<std::vector<std::string>>
  splitByPipe(const std::vector<std::string> &args) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> current;

    for (const auto &arg : args) {
      if (arg == "|") {
        if (!current.empty()) {
          result.push_back(current);
          current.clear();
        }
      } else {
        current.push_back(arg);
      }
    }
    if (!current.empty()) {
      result.push_back(current);
    }
    return result;
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

      std::vector<std::vector<std::string>> pipelineCmds = splitByPipe(args);

      if (pipelineCmds.size() > 1) {
        PipelineExecutor::execute(pipelineCmds,
                                  [this](const std::vector<std::string> &a) {
                                    return executeCommand(a);
                                  });
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

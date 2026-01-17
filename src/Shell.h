#pragma once

#include "commands/Command.h"
#include "commands/ExternalCommand.h"
#include "parsers/CommandParser.h"
#include "utils/IORedirector.h"
#include "utils/PathSearcher.h"
#include <iostream>
#include <memory>
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

  void run() {
    while (true) {
      std::cout << "$ ";

      std::string input;
      if (!std::getline(std::cin, input)) {
        break;
      }

      if (input.empty())
        continue;

      std::pair<std::vector<std::string>, Redirection> parsed =
          CommandParser::parseWithRedirection(input);
      std::vector<std::string> args = parsed.first;
      Redirection redir = parsed.second;

      if (args.empty())
        continue;

      IORedirector ioRedirector;
      if (redir.hasOutputRedirect) {
        ioRedirector.redirectOutput(redir.outputFile, redir.appendOutput);
      }
      if (redir.hasErrorRedirect) {
        ioRedirector.redirectError(redir.errorFile, redir.appendError);
      }

      std::string cmdName = args[0];
      if (hasCommand(cmdName)) {
        commands[cmdName]->execute(args);
      } else {
        static ExternalCommand externalCmd;
        externalCmd.execute(args);
      }

      ioRedirector.restore();
    }
  }
};

#pragma once

#include "commands/Command.h"
#include "commands/ExternalCommand.h"
#include "parsers/CommandParser.h"
#include "utils/IORedirector.h"
#include "utils/PathSearcher.h"
#include <iostream>
#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <unordered_map>
#include <vector>

char *command_generator(const char *text, int state) {
  static std::vector<std::string> matches;
  static size_t match_index = 0;

  if (state == 0) {
    matches.clear();
    match_index = 0;
    std::vector<std::string> builtins = {"echo", "exit"};
    std::string text_str(text);
    for (const auto &word : builtins) {
      if (word.substr(0, text_str.size()) == text_str) {
        matches.push_back(word);
      }
    }
  }

  if (match_index < matches.size()) {
    std::string res = matches[match_index++];
    return strdup(res.c_str());
  }

  return nullptr;
}

char **attempted_completion(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, command_generator);
}

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
    rl_attempted_completion_function = attempted_completion;

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

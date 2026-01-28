#pragma once

#include "commands/Command.h"
#include "commands/ExternalCommand.h"
#include "parsers/CommandParser.h"
#include "utils/Autocompleter.h"
#include "utils/IORedirector.h"
#include "utils/PathSearcher.h"
#include "utils/PipelineExecutor.h"
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <unordered_map>
#include <vector>

class Shell {
private:
  std::vector<std::string> cmdHistory;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
  size_t historyWrittenCount = 0;
  std::string historyPath;

public:
  void registerCommands(Command *cmd) {
    commands[cmd->getName()] = std::unique_ptr<Command>(cmd);
  }

  bool hasCommand(const std::string &name) const {
    return commands.find(name) != commands.end();
  }

  const std::vector<std::string> &getHistory() const { return cmdHistory; }

  void appendToHistory(const std::string &cmd) {
    if (cmd.empty())
      return;
    add_history(cmd.c_str());
    cmdHistory.push_back(cmd);
  }

  std::string getDefaultHistoryPath() {
    const char *histfile = getenv("HISTFILE");
    if (histfile) {
      return std::string(histfile);
    }
    const char *home = getenv("HOME");
    if (home) {
      return std::string(home) + "/.shell_history";
    }
    return ".shell_history";
  }

  void loadHistory(const std::string &path = "") {
    historyPath = path.empty() ? getDefaultHistoryPath() : path;
    std::ifstream historyFile(historyPath);
    if (historyFile.is_open()) {
      std::string line;
      while (std::getline(historyFile, line)) {
        if (!line.empty()) {
          add_history(line.c_str());
          cmdHistory.push_back(line);
        }
      }
      historyWrittenCount = cmdHistory.size();
    }
  }

  void saveHistory(const std::string &path = "") {
    std::string targetPath = path.empty() ? historyPath : path;
    if (targetPath.empty()) {
      targetPath = getDefaultHistoryPath();
    }

    std::ofstream historyFile(targetPath, std::ios::app);
    if (historyFile.is_open()) {
      for (size_t i = historyWrittenCount; i < cmdHistory.size(); ++i) {
        historyFile << cmdHistory[i] << std::endl;
      }
      historyWrittenCount = cmdHistory.size();
    }
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
    loadHistory();

    while (true) {
      char *input_c = readline("$ ");

      if (!input_c) {
        saveHistory();
        break;
      }

      std::string input(input_c);
      if (input.length() > 0) {
        add_history(input_c);
        cmdHistory.push_back(input);
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

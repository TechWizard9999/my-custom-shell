#include "Shell.h"
#include "commands/CdCommand.h"
#include "commands/EchoCommand.h"
#include "commands/ExitCommand.h"
#include "commands/HistoryCommand.h"
#include "commands/PwdCommand.h"
#include "commands/TypeCommand.h"
#include <iostream>

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  Shell shell;

  shell.registerCommands(new ExitCommand(shell));
  shell.registerCommands(new EchoCommand());
  shell.registerCommands(new TypeCommand(shell));
  shell.registerCommands(new PwdCommand());
  shell.registerCommands(new CdCommand());
  shell.registerCommands(new HistoryCommand(shell));
  shell.run();

  return 0;
}

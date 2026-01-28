#include "../Shell.h"

class ExitCommand : public Command {
private:
  Shell &shell;

public:
  ExitCommand(Shell &s) : shell(s) {}

  std::string getName() const override { return "exit"; }

  int execute(const std::vector<std::string> &args) override {
    shell.saveHistory();
    std::exit(0);
    return 0;
  }
};

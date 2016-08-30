#ifndef HOME_CTRL_HPP
#define HOME_CTRL_HPP

#include "Command.hpp"

class HomeCtrl : public Command
{
public:
    HomeCtrl();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int col_;
};

#endif // HOME_CTRL_HPP
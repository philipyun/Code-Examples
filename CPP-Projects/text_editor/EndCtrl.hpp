#ifndef END_CTRL_HPP
#define END_CTRL_HPP

#include "Command.hpp"

class EndCtrl : public Command
{
public:
    EndCtrl();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int col_;
};

#endif // END_CTRL_HPP
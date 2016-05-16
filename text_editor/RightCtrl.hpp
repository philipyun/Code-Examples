#ifndef RIGHT_CTRL_HPP
#define RIGHT_CTRL_HPP

#include "Command.hpp"


class RightCtrl : public Command
{
public:
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);
};


#endif // RIGHT_CTRL_HPP
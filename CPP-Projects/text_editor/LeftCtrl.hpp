#ifndef LEFT_CTRL_HPP
#define LEFT_CTRL_HPP

#include "Command.hpp"

class LeftCtrl : public Command
{
public:
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);
};


#endif // LEFT_CTRL_HPP
#ifndef DOWN_CTRL_HPP
#define DOWN_CTRL_HPP

#include "EditorModel.hpp"
#include "Command.hpp"

class DownCtrl : public Command
{
public:
    DownCtrl();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int col_;
};

#endif // DOWN_CTRL_HPP
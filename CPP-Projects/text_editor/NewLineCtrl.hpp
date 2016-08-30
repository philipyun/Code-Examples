#ifndef NEWLINE_CTRL_HPP
#define NEWLINE_CTRL_HPP

#include "Command.hpp"
#include "EditorModel.hpp"

class NewLineCtrl : public Command
{
public:
    NewLineCtrl();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int row_;
    int col_;
};


#endif // NEWLINE_CTRL_HPP
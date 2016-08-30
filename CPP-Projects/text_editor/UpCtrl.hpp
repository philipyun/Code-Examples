#ifndef UP_CTRL_HPP
#define UP_CTRL_HPP

#include "EditorModel.hpp"
#include "Command.hpp"

class UpCtrl : public Command
{
public:
    UpCtrl();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int col_;
};

#endif // UP_CTRL_HPP
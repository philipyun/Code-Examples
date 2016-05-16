#ifndef DELETE_LINE_HPP
#define DELETE_LINE_HPP

#include "Command.hpp"
#include "EditorModel.hpp"

class DeleteLine : public Command
{
public:
    DeleteLine();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    int row_;
    int col_;
    int lines_;

    std::string deletedLine_;
};

#endif // DELETE_LINE_HPP
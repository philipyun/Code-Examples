#ifndef DELETE_CHAR_HPP
#define DELETE_CHAR_HPP

#include "EditorModel.hpp"
#include "Command.hpp"

class DeleteChar : public Command
{
public:
    DeleteChar();
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    char character;

    int row_;
    int col_;
};

#endif // DELETE_CHAR_HPP
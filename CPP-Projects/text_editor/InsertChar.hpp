#ifndef INSERT_CHAR_HPP
#define INSERT_CHAR_HPP

#include "EditorModel.hpp"
#include "Command.hpp"

class InsertChar : public Command
{
public:
    InsertChar(char a);
    virtual void execute(EditorModel& model);
    virtual void undo(EditorModel& model);

private:
    char character;
};


#endif // INSERT_CHAR_HPP
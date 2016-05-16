#include "DeleteChar.hpp"
#include "InsertChar.hpp"
#include "RightCtrl.hpp"

InsertChar::InsertChar(char a)
    : character{a}
{
}


void InsertChar::execute(EditorModel& model)
{   
    RightCtrl right;
    int insertCol = model.cursorColumn() - 1;

    std::string& temp = model.line(model.cursorLine());
    temp = temp.substr(0, insertCol) + character + temp.substr(insertCol);

    right.execute(model);
}


void InsertChar::undo(EditorModel& model)
{
    DeleteChar dchar;
    dchar.execute(model);
}
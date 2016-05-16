#include "NewLineCtrl.hpp"

NewLineCtrl::NewLineCtrl()
    : row_{0}, col_{0}
{
}


void NewLineCtrl::execute(EditorModel& model)
{
    row_ = model.cursorLine();
    col_ = model.cursorColumn();

    std::string temp = model.line(row_);
    std::string prev = temp.substr(0, col_ - 1);
    std::string next = temp.substr(col_ - 1);

    model.line(row_) = prev;
    model.insertLine(next);

    model.moveCursorDown();
    model.setCursorHome();
}


void NewLineCtrl::undo(EditorModel& model)
{
    std::string right = model.line(model.cursorLine());
    model.line(model.cursorLine() - 1) += right;
    model.deleteLine();

    model.setRow(row_);
    model.setCol(col_);
}
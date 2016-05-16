#include "DeleteLine.hpp"
#include "NewLineCtrl.hpp"
#include "EditorException.hpp"

namespace
{
    void setCursorAfterDelete(EditorModel& model, int col)
    {
        if (model.lineCount() == model.cursorLine() - 1)
        {
            model.moveCursorUp();
        }

        if (col > model.line(model.cursorLine()).size())
        {
            model.setCursorEnd();
        }
    }
}


DeleteLine::DeleteLine()
    : row_{0}, col_{0}, lines_{0}, deletedLine_{""}
{
}


void DeleteLine::execute(EditorModel& model)
{
    row_ = model.cursorLine();
    col_ = model.cursorColumn();
    lines_ = model.lineCount();

    deletedLine_ = model.line(row_);

    if (model.lineCount() == 1)
    {
        if (model.line(1) == "")
        {
            throw EditorException("Already Empty");
        }
        else
        {
            model.line(1) = "";
            model.setCursorHome();
        }
        return;
    }

    model.deleteLine();
    setCursorAfterDelete(model, col_);
}


void DeleteLine::undo(EditorModel& model)
{
    if (lines_ == 1 && model.line(1) == "")
    {
        model.line(1) = deletedLine_;
    }
    else
    {
        model.setRow(row_ - 1);
        model.insertLine(deletedLine_);
    }
    model.setRow(row_);
    model.setCol(col_);
}
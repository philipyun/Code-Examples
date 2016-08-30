#include "EditorException.hpp"
#include "DeleteChar.hpp"
#include "LeftCtrl.hpp"
#include "InsertChar.hpp"

#include "BooEditLog.hpp"

DeleteChar::DeleteChar()
    : character{'0'}, row_{0}, col_{0}
{
}


void DeleteChar::execute(EditorModel& model)
{
    LeftCtrl left;
    row_ = model.cursorLine();
    col_ = model.cursorColumn();

    std::string& temp = model.line(row_);

    if (col_ > 1)
    {
        character = temp[col_ - 2];
        temp.erase(temp.begin() + col_ - 2);
    }
    else if (col_ == 1)
    {
        if (row_ != 1)
        {
            character = '\r';
            int shiftLeft = temp.size();

            model.line(row_ - 1) += temp;
            model.deleteLine();

            for (int i = 0; i < shiftLeft; i++)
            {
                left.execute(model);
            }
        }
        else
        {
            throw EditorException("Already at beginning");
        }
    }
    left.execute(model);
}


void DeleteChar::undo(EditorModel& model)
{
    if (character == '\r')
    {
        std::string& temp = model.line(model.cursorLine());
        std::string left = temp.substr(0, model.cursorColumn() - 1);
        std::string right = temp.substr(model.cursorColumn() - 1);

        temp = left;
        model.insertLine(right);

        model.setRow(row_);
        model.setCol(col_);
    }
    else
    {
        InsertChar iChar(character);
        iChar.execute(model);
    }
}
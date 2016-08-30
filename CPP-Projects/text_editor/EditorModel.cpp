// EditorModel.cpp
//
// ICS 45C Fall 2015
// Project #4: People Just Love to Play with Words
//
// Implementation of the EditorModel class

#include "EditorModel.hpp"
#include "EditorException.hpp"


EditorModel::EditorModel()
    : cursorRow{1}, cursorCol{1}, cErrMsg{"Welcome to BooEdit"}, lines{{""}}
{
}


int EditorModel::cursorLine() const
{
    return cursorRow;
}


int EditorModel::cursorColumn() const
{
    return cursorCol;
}


int EditorModel::lineCount() const
{
    return lines.size();
}


const std::string& EditorModel::line(int lineNumber) const
{
    return lines[lineNumber-1];
}


const std::string& EditorModel::currentErrorMessage() const
{
    return cErrMsg;
}


void EditorModel::setErrorMessage(const std::string& errorMessage)
{
    cErrMsg = errorMessage;
}


void EditorModel::clearErrorMessage()
{
    cErrMsg = "";
}


void EditorModel::setCursorHome()
{
    cursorCol = 1;
}


void EditorModel::setCursorEnd()
{
    cursorCol = line(cursorRow).size() + 1;
}


void EditorModel::setRow(int row)
{
    cursorRow = row;
}


void EditorModel::setCol(int col)
{
    cursorCol = col;
}


void EditorModel::moveCursorUp()
{
    cursorRow--;
}


void EditorModel::moveCursorDown()
{
    cursorRow++;
}


void EditorModel::moveCursorLeft()
{
    cursorCol--;
}


void EditorModel::moveCursorRight()
{
    cursorCol++;
}


void EditorModel::insertLine(std::string next)
{
    lines.insert(lines.begin() + cursorLine(), next);
}


void EditorModel::deleteLine()
{
    lines.erase(lines.begin() + cursorLine() - 1);
}


std::string& EditorModel::line(int lineNumber)
{
    return lines[lineNumber-1];
}
// KeypressInteractionReader.cpp
//
// ICS 45C Fall 2015
// Project #4: People Just Want to Play with Words
//
// Implementation of the KeypressInteractionReader
//
// YOU WILL NEED TO IMPLEMENT SOME THINGS HERE

#include "KeypressInteractionReader.hpp"

#include "UpCtrl.hpp"
#include "DownCtrl.hpp"
#include "LeftCtrl.hpp"
#include "RightCtrl.hpp"

#include "HomeCtrl.hpp"
#include "EndCtrl.hpp"

#include "InsertChar.hpp"
#include "NewLineCtrl.hpp"
#include "DeleteChar.hpp"
#include "DeleteLine.hpp"

#include "BooEditLog.hpp"


// You will need to update this member function to watch for the right
// keypresses and build the right kinds of Interactions as a result.
//
// The reason you'll need to implement things here is that you'll need
// to create Command objects, which you'll be declaring and defining
// yourself; they haven't been provided already.
//
// I've added handling for "quit" here, but you'll need to add the others.

Interaction KeypressInteractionReader::nextInteraction()
{
    while (true)
    {
        Keypress keypress = keypressReader.nextKeypress();
        Command* cmd = nullptr;

        std::string a = "";
        a += keypress.code();

        if (keypress.ctrl())
        {
            // The user pressed a Ctrl key (e.g., Ctrl+X); react accordingly
            switch (keypress.code())
            {
            case 'X':
                return Interaction::quit();
            case 'Z':
                return Interaction::undo();
            case 'A':
                return Interaction::redo();
            case 'I':
                cmd = new UpCtrl;
                break;
            case 'K':
                cmd = new DownCtrl;
                break;
            case 'U':
                cmd = new LeftCtrl;
                break;
            case 'O':
                cmd = new RightCtrl;
                break;
            case 'Y':
                cmd = new HomeCtrl;
                break;
            case 'P':
                cmd = new EndCtrl;
                break;
            case 'J':
            case 'M':
                cmd = new NewLineCtrl;
                break;
            case 'H':
                cmd = new DeleteChar;
                break;
            case 'D':
                cmd = new DeleteLine;
                break;
            default:
                continue;
            }
            booEditLog("Ctrl + " + a);
            return Interaction::command(cmd);
        }
        else
        {
            // The user pressed a normal key (e.g., 'h') without holding
            // down Ctrl; react accordingly
            booEditLog("Insert + " + a);
            return Interaction::command(new InsertChar(keypress.code()));
        }
    }
}


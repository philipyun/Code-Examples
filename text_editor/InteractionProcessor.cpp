// InteractionProcessor.cpp
//
// ICS 45C Fall 2015
// Project #4: People Just Love to Play with Words
//
// Implementation of the InteractionProcessor class

#include "InteractionProcessor.hpp"
#include "EditorException.hpp"
#include "Interaction.hpp"

#include <stack>
#include "BooEditLog.hpp"

// This function implements command execution, but does not handle "undo"
// and "redo"; you'll need to add that handling.
namespace
{
    void clearDynamicStack(std::stack<Command*>& cmdStack)
    {
        while (!cmdStack.empty())
        {
            Command* temp = cmdStack.top();
            cmdStack.pop();
            delete temp;
        }
    }
}


void InteractionProcessor::run()
{
    view.refresh();

    std::stack<Command*> UndoStack;
    std::stack<Command*> RedoStack;

    while (true)
    {
        view.refresh();
        Interaction interaction = interactionReader.nextInteraction();
                
        //model.clearErrorMessage();

        if (interaction.type() == InteractionType::quit)
        {
            clearDynamicStack(UndoStack);
            clearDynamicStack(RedoStack);
            break;
        }
        else if (interaction.type() == InteractionType::undo)
        {
            if (!UndoStack.empty())
            {
                UndoStack.top()->undo(model);

                RedoStack.push(UndoStack.top());
                UndoStack.pop();
            }
            view.refresh();
        }
        else if (interaction.type() == InteractionType::redo)
        {
            if (!RedoStack.empty())
            {
                RedoStack.top()->execute(model);

                UndoStack.push(RedoStack.top());
                RedoStack.pop();
            }
            view.refresh();
        }
        else if (interaction.type() == InteractionType::command)
        {
            Command* command = interaction.command();

            try
            {
                command->execute(model);
                model.clearErrorMessage();
            }
            catch (EditorException& e)
            {
                model.setErrorMessage(e.getReason());
                
                delete command;
                continue;
                view.refresh();
            }
            UndoStack.push(command);
            clearDynamicStack(RedoStack);

            view.refresh();

            // Note that you'll want to be more careful about when you delete
            // commands once you implement undo and redo.  For now, since
            // neither is implemented, I've just deleted it immediately
            // after executing it.  You'll need to wait to delete it until
            // you don't need it anymore.
            
            // delete command;
        }
        model.clearErrorMessage();
    }
}


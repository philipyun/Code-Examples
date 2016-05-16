__author__ = 'Philip Yun 65035200'
# ICS 32, Lab 2 (10AM)

import tkinter


#####################################
# ALL FUNCTIONS/METHODS RETURN NONE #
#####################################


QUERY_FONT = ('Helvetica', 14, 'bold')



class OthelloSettings:
    def __init__(self):
        ''' creates the windows for the othello settings
        '''

        self.dimensions = 0, 0

        self._start_window = tkinter.Tk()
        self._start_window.update()
        self._start_window.minsize(400, 250)

        self._start_window.wm_title('Othello Settings')
        
        self._startWindow()

        self._start_window.rowconfigure(0, weight = 1)
        self._start_window.rowconfigure(1, weight = 1)        
        self._start_window.rowconfigure(2, weight = 1)
        self._start_window.rowconfigure(3, weight = 1)
        self._start_window.rowconfigure(4, weight = 1)
        self._start_window.rowconfigure(5, weight = 1)
        self._start_window.columnconfigure(0, weight = 1)
        self._start_window.columnconfigure(1, weight = 1)
        self._start_window.columnconfigure(2, weight = 1)



    def _startWindow(self):
        ''' runs all settings for the start window
        '''

        self._startDimensions()
        self._startDiscs()
        self._startWinMode()
        self._startButtons()



    def _startDimensions(self):
        ''' asks the user for the dimensions of the board
        '''

        tkinter.Label(
            self._start_window, 
            text = 'Row Size:',
            font = QUERY_FONT).grid(
                row = 0, 
                column = 0,
                sticky = tkinter.W,
                padx = 10)

        tkinter.Label(
            self._start_window, 
            text = 'Column Size:',
            font = QUERY_FONT).grid(
                row = 1, 
                column = 0,
                sticky = tkinter.W, 
                padx = 10)

        self._numRowsEntry = tkinter.Scale(
            self._start_window, 
            from_ = 4, to = 16, 
            resolution = 2, 
            orient = tkinter.HORIZONTAL)
        self._numRowsEntry.set(8)

        self._numRowsEntry.grid(
            row = 0, 
            column = 1, 
            columnspan = 2,
            padx = 10,
            sticky = tkinter.W + tkinter.E)
        
        self._numColsEntry = tkinter.Scale(
            self._start_window,
            from_ = 4, to = 16, 
            resolution = 2, 
            orient = tkinter.HORIZONTAL)
        self._numColsEntry.set(8)

        self._numColsEntry.grid(
            row = 1, 
            column = 1, 
            columnspan = 2, 
            padx = 10,
            sticky = tkinter.W + tkinter.E)



    def _startDiscs(self):
        ''' asks the user for the upper left disc
            and the first player
        '''

        self._firstPlayEntry = tkinter.IntVar()
        self._startDiscEntry = tkinter.IntVar()

        tkinter.Label(
            self._start_window,
            text = 'First Player:',
            font = QUERY_FONT).grid(
                row = 2, 
                column = 0,
                sticky = tkinter.W, 
                padx = 10,
                pady = 10)

        tkinter.Label(
            self._start_window,
            text = 'Upper Left Corner:',
            font = QUERY_FONT).grid(
                row = 3, 
                column = 0, 
                sticky = tkinter.W, 
                padx = 10,
                pady = 10)
        
        tkinter.Radiobutton(
            self._start_window, 
            variable = self._firstPlayEntry, 
            text = "Black", 
            value = 1,
            padx = 10).grid(row = 2, 
                column = 1, 
                sticky = tkinter.W)

        tkinter.Radiobutton(
            self._start_window, 
            variable = self._firstPlayEntry, 
            text = "White", 
            value = 2,
            padx = 10).grid(
                row = 2, 
                column = 2, 
                sticky = tkinter.W,
                padx = 10)

        tkinter.Radiobutton(
            self._start_window, 
            variable = self._startDiscEntry,
            text = "Black", 
            value = 1,
            padx = 10).grid(
                row = 3, 
                column = 1, 
                sticky = tkinter.W)

        tkinter.Radiobutton(
            self._start_window, 
            variable = self._startDiscEntry, 
            text = "White", 
            value = 2,
            padx = 10).grid(
                row = 3, 
                column = 2, 
                sticky = tkinter.W,
                padx = 10)

        self._firstPlayEntry.set(1)
        self._startDiscEntry.set(1)

        
    
    def _startWinMode(self):
        ''' asks user for the win mode
        '''

        self.winModeEntry = tkinter.StringVar()

        tkinter.Label(
            self._start_window,
            text = 'Win Mode:',
            font = QUERY_FONT).grid(
                row = 4, 
                column = 0,
                sticky = tkinter.W,
                padx = 10,
                pady = 10)

        tkinter.Radiobutton(
            self._start_window, 
            variable = self.winModeEntry,
            text = "More Discs", 
            value = ">",
            padx = 10).grid(
                row = 4, 
                column = 1,
                sticky = tkinter.W)

        tkinter.Radiobutton(
            self._start_window, 
            variable = self.winModeEntry,
            text = "Less Discs", 
            value = "<",
            padx = 10).grid(
                row = 4, 
                column = 2, 
                sticky = tkinter.W,
                padx = 10)

        self.winModeEntry.set(">")



    def _startButtons(self):
        ''' gives button options for the settings page
        '''

        buttonFrame = tkinter.Frame(self._start_window, 
            padx = 10, pady = 10)

        self.playButton = tkinter.Button(
            buttonFrame, 
            text='Play!',
            command = self._playButtonPressed)

        self.cancelButton = tkinter.Button(
            buttonFrame, 
            text='Quit :(',
            command = self.closeWindow)

        self.playButton.grid(
            row = 0, 
            column = 0,
            padx = 15)

        self.cancelButton.grid(
            row = 0, 
            column = 1,
            padx = 15)

        buttonFrame.grid(
            row = 5,
            column = 1,
            columnspan = 2)


    
    def _playButtonPressed(self):
        ''' event handling for the play button
        '''
        self.dimensions = (int(self._numRowsEntry.get()), 
            int(self._numColsEntry.get()))
        self.firstPlay = self._firstPlayEntry.get()
        
        if self._startDiscEntry.get() == 1:
            self.startDisc = [1, 2]
        else:
            self.startDisc = [2, 1]

        self.winMode = self.winModeEntry.get()
        self.closeWindow()



    def closeWindow(self):
        ''' closes the window when need be
        '''

        self._start_window.destroy()



    def start(self):
        ''' starts the settings window
        '''
        self._start_window.mainloop()
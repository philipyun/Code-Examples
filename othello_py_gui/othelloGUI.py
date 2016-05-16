__author__ = 'Philip Yun 65035200'
# ICS 32, Lab 2 (10AM)

import math
import othello
import othelloSettings
import tkinter
from tkinter import messagebox


################################
# ALL FUNCTIONS/METHODS RETURN #
# NONE UNLESS OTHERWISE STATED #
################################



class OthelloApplication:
    def __init__(self, settings):
        ''' __init__ creates game settings, the framework
            and window needed to make the game
            function as desired
        '''

        ######## Game Attributes ########

        self.discs = []

        self._settings = settings

        try:
            self._createGameFramework()
            self._createGameWindow()

        except:
            pass

            

    ######## Window Handling ########

    def _createBlankBoard(self):
        ''' Creates checker board
        '''
        width, height = self._getWinDimensions()
        
        for i in range(self.numRows):

            for j in range(self.numCols):
                
                if (i + j) % 2 == 0:
                    color = '#336600'
                
                else:
                    color = '#4C9900'

                rowFract_ul = (j)/self.numCols
                colFract_ul = (i)/self.numRows

                rowFract_br = (j+1)/self.numCols
                colFract_br = (i+1)/self.numRows

                self._canvas.create_rectangle(
                    rowFract_ul * width + 3, 
                    colFract_ul * height + 3,
                    rowFract_br * width + 3, 
                    colFract_br * height + 3,
                    fill = color, 
                    outline = color)



    def _createGameWindow(self):
        ''' creates window and all of its attribtues.
            Event handling included here
        '''

        ######## Window/Canvas Attributes ########

        self._window = tkinter.Tk()
        self._window.wm_title('Othello')

        autoSize = 45
        if self.numRows == 4:
            autoSize = 65

        self._createScoreBoard()


        ######## Canvas Handling ########

        self._canvas = tkinter.Canvas(
            master = self._window, 
            width = self.numCols * autoSize, 
            height = self.numRows * autoSize)

        self._canvas.grid(
            row = 1, 
            column = 0,
            columnspan = 4,
            sticky = (
                tkinter.N + 
                tkinter.S + 
                tkinter.E + 
                tkinter.W))

        ######## Event Handling ########

        self._canvas.bind('<Motion>', self._onHover)
        self._canvas.bind('<Button-1>', self._onClick)
        self._canvas.bind('<Configure>', self._onResize)
        self._canvas.bind('<ButtonRelease-1>', self._onRelease)

        self._canvas.update()
        self._window.minsize(
            self._canvas.winfo_width(),
            self._canvas.winfo_height() + 22)

        #### Minsize set so that labels dont dissapear. Ever.


        ######## Other Stuff ########

        self._window.rowconfigure(0, weight = 1)
        self._window.rowconfigure(1, weight = 1)
        self._window.columnconfigure(0, weight = 1)
        self._window.columnconfigure(1, weight = 1)
        self._window.columnconfigure(2, weight = 1)
        self._window.columnconfigure(3, weight = 1)

        self._validOutline = ''



    def _createScoreBoard(self):
        ''' Creates the score board and the turn display
        '''

        self._blackScoreCount = tkinter.Label(
            master = self._window,
            text = 'Black: {}'.format(
                self.gameState.points[0]))

        self._blackScoreCount.grid(
            row = 0,
            column = 0,
            sticky = tkinter.W)


        self._currentTurn = tkinter.Label(
            master = self._window,
            text = 'Turn: {}'.format(
                ['Black', 'White'][self.gameState.player.turn - 1]))

        self._currentTurn.grid(
            row = 0,
            column = 1,
            columnspan = 2,
            sticky = tkinter.W + tkinter.E)
        

        self._whiteScoreCount = tkinter.Label(
            master = self._window,
            text = 'White: {}'.format(
                self.gameState.points[1]))

        self._whiteScoreCount.grid(
            row = 0,
            column = 3,
            sticky = tkinter.E)



    def start(self):
        ''' attempts to start the game.
            if settings are invalid, window
            terminates
        '''

        try:
            self._window.mainloop()
        except AttributeError:
            pass



    ######## GameState Handling ########

    def _createGameFramework(self):
        ''' retrieves settings and gamestate attributes
            and stores it in a general framework
        '''

        self.numRows, self.numCols = self._settings.dimensions

        self.gameState = othello.GameState(
            self.numRows, 
            self.numCols, 
            othello.Player(
                self._settings.firstPlay, 
                self._settings.startDisc), 
            self._settings.winMode)



    def _noMovesAvailable(self):
        ''' displays a message box if a player has no valid 
            moves
        '''

        color = ['Black', 'White'][self.gameState.player.turn - 1]
        
        msg = ('{} has no available moves! '.format(color) +
              '\nNext player\'s turn')

        messagebox.showinfo('ALERT', msg)

        self.gameState.player.nextTurn()
        self._updateCounter()



    def _gameOver(self):
        ''' instructions on what to do when a game over error is
            raised
        '''

        winner = self.gameState._winnerIs()
        
        blackPoints, whitePoints = self.gameState.points
        scoreBoard = 'Black: {}, White: {}'.format(blackPoints, whitePoints)
        
        if winner == 'NONE':
            dialog = 'TIE! '

        else:
            color = ['Black', 'White'][winner - 1]
            dialog = '{} Wins! '.format(color)


        if messagebox.askyesno('GAME OVER', 
            dialog + scoreBoard + '\nPlay Again?'):

            self._window.destroy()

            self._settings = othelloSettings.OthelloSettings()
            self._settings.start()

            self.__init__(self._settings)
        
        else:
            self._window.destroy()



    ######### Event Based Handling #########

    def _onResize(self, event):
        ''' rezise window event handling
        '''

        self._canvas.delete(tkinter.ALL) 
        self._createBlankBoard()
        self._placeDiscs()



    def _onHover(self, event):
        ''' mouse hover window event handling
        '''

        x, y = self._getBoardCoord(event)
        self._checkValid(x, y)



    def _onClick(self, event):
        ''' mouse click event handling
        '''

        self._canvas.delete(self._validOutline)
        
        x, y = self._getBoardCoord(event)
        self._makeMove(x, y)



    def _onRelease(self, event):
        ''' mouse click release event handling
        '''

        try:
            self.gameState.checkGameState()

        except othello.NoMovesAvailableError:
            self._noMovesAvailable()

        except othello.GameOverError:
            self._gameOver()



    ######## Move Handling ########

    def _checkValid(self, x, y):
        ''' if a valid move is in existence, a rectangle will
            appear over the hovered coordinate
        '''

        move = [x - 1, y -1]

        ulx, uly, brx, bry = self._getDiscCoord(x, y)

        self._canvas.delete(self._validOutline)

        if self.gameState.player.checkMove(self.gameState, move):
            self._validOutline = self._canvas.create_rectangle(
                ulx,
                uly,
                brx,
                bry,
                fill = '',
                outline = 'red')



    def placeDisc(self, x, y, fill, outline):
        ''' disc placement for the game
        '''

        width, height = self._getWinDimensions()

        centerX, centerY = self._getDiscCenter(x, y)

        ulx, uly, brx, bry = self._getDiscCoord(x, y)

        tkDisc = self._canvas.create_oval(
            ulx,
            uly,
            brx,
            bry,
            fill = fill,
            outline = outline)

        disc = OthelloDisc(x - 1, y - 1, centerX, centerY, tkDisc)

        self.discs.append(disc)



    def _placeDiscs(self):
        ''' places all discs on the board
        '''

        board = self.gameState.gameBoard.board
        for i in range(len(board)):
            
            for j in range(len(board[0])):
                
                if board[i][j] != 0:
                    
                    if board[i][j] == 1:
                        fill = 'black'
                        outline = 'white'
                    
                    elif board[i][j] == 2:
                        fill = 'white'
                        outline = 'black'
                    
                    self.placeDisc(i+1, j+1, fill, outline)



    def _makeMove(self, x, y ):
        ''' makes move on the gameboard
        '''

        move = [x - 1, y - 1]

        if self.gameState.player.checkMove(self.gameState, move):

            fill, outline = self._getShades()
            self.placeDisc(x, y, fill, outline)

            self._capture(self.gameState.player.makeMove(
                self.gameState,
                move))

            self._updateCounter()



    def _capture(self, captureCoords):
        ''' captures the tiles that are captured
            in a move execution
        '''

        for disc in self.discs:
            
            if disc.boardCoord in captureCoords:
                fill, outline = self._getShades()
                
                self._canvas.itemconfig(
                    disc.tkobject, 
                    fill = outline, 
                    outline = fill)



    def _updateCounter(self):

        self._blackScoreCount.config(
            text = 'Black: {}'.format(
                self.gameState.points[0]))

        self._currentTurn.config(
            text = 'Turn: {}'.format(
                ['Black', 'White'][self.gameState.player.turn - 1 ]))

        self._whiteScoreCount.config(
            text = 'White: {}'.format(
                self.gameState.points[1]))



    ######## tkinter object Handling and info retrieval ########

    def _getWinDimensions(self) -> tuple:
        ''' returns the dimensions of the canvas
        '''

        self._window.update()

        width = self._canvas.winfo_width() - 6
        height = self._canvas.winfo_height() - 6

        return width, height
    


    def _getBoardCoord(self, event) -> list:
        ''' returns the board translated coordinates of a 
            click event
        '''

        width, height = self._getWinDimensions()

        x = math.ceil(event.y/height * self.numRows)
        y = math.ceil(event.x/width * self.numCols)

        coord = [x, y]

        if not self.gameState.player._inRange(x, y, self.gameState):
            
            for i in range(len(coord)):
                
                if coord[i] == 0:
                    coord[i] += 1
                
                else:
                    
                    if i == 0:
                        
                        if coord[i] == self.numRows + 1:
                            coord[i] = self.numRows
                    
                    if i == 1:
                        
                        if coord[i] == self.numCols + 1:
                            coord[i] = self.numCols

        return coord



    def _getDiscCoord(self, x, y) -> tuple:
        ''' creates disc coordinates to use for the 
            create_shape methods

            ulx = upper left x
            brx = bottom right x

            etc.
        '''

        width, height = self._getWinDimensions()
        centerX, centerY = self._getDiscCenter(x, y)

        radiusX = width/self.numCols/2 * 7/8
        radiusY = height/self.numRows/2 * 7/8

        ulx = centerX - radiusX
        uly = centerY - radiusY

        brx = centerX + radiusX
        bry = centerY + radiusY

        return ulx, uly, brx, bry



    def _getDiscCenter(self, x, y) -> tuple:
        ''' Returns the coordinates of the theoretical center 
            coordinate of a disc
        '''
        width, height = self._getWinDimensions()

        centerX = (2 * y - 1)/2 * width/self.numCols + 3
        centerY = (2 * x - 1)/2 * height/self.numRows + 3

        return centerX, centerY



    def _getShades(self) -> tuple:
        ''' gets the shades and outlines needed for the shapes
            being drawn
        '''

        if self.gameState.player.turn == 1:
            return 'black', 'white'
        
        return 'white', 'black'



class OthelloDisc:
    def __init__(self, x, y, centerx, centery, tkobject):
        self.boardCoord = x, y
        self.centerCoord = 0
        self.tkobject = tkobject



if __name__ == '__main__':
    settings = othelloSettings.OthelloSettings()
    settings.start()
    OthelloApplication(settings).start()
    

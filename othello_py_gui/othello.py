__author__ = 'Philip Yun 65035200'
# ICS 32, Lab 2 (10AM)

class InvalidMoveError(Exception):
    pass



class GameOverError(Exception):
    pass



class NoMovesAvailableError(Exception):
    pass



class GameBoard:
    ''' Production of an Othello GameBoard. Dimensions of the
        board will be asked when GameBoard() is called.
    '''

    def __init__(self, numRows, numCols):
        ''' The __init__ mehthod will produce a new game board with
            dimension.
        '''
        self.numRows = numRows
        self.numCols = numCols
        self.board = self.blankGameBoard()



    def blankGameBoard(self) -> None:
        ''' Stores a clean board based on the given dimensions in the
            object's attributes.  There are no pieces on the board.
            It is completely blank
        '''

        board = []
        
        for i in range(self.numRows):
            colList = []
            
            for j in range(self.numCols):
                colList.append(0)

            board.append(colList)

        return board



class GameState:
    ''' Calling GameState() will produce an Othello Game State 
        object.  The GameState has two major attributes:
            
            1.   A GameBoard Object
            2.   A Player Object

        The Player object counts turns and handles all moves
        made by the players.  Although two players are required
        for the game, both players are represented by one object.
        Methods on the player object handle turn exchanges made by
        the two different players.

        Other attributes include:

            1.   Game Over Status (Is the game over?)
            2.   Point Handling
    '''

    def __init__(self, numRows, numCols, player, mode):
        ''' The __init__ method essentially starts a new game.
        '''

        self.gameBoard = GameBoard(numRows, numCols)     # GameBoard Object
        self.player = player                             # Player Object
        self.gameMode = mode
        
        self.points = (0, 0)
        self._gameOver = False
        
        self._newBoard()


    ############ NEW GAME HANDLING ############

    def _newBoard(self) -> None:
        ''' Produces a new board
        '''

        rows = self.gameBoard.numRows
        cols = self.gameBoard.numCols

        for x in range(rows):

            for y in range(cols):

                if ((x == (rows/2 - 1) and y == (cols/2 - 1)) 
                    or (x == (rows/2)) and y == (cols/2)):
                    self.gameBoard.board[x][y] = self.player.firstTile[0]

                elif ((x == (rows/2 - 1) and y == (cols/2)) 
                    or (x == (rows/2)) and y == (cols/2 - 1)):
                    self.gameBoard.board[x][y] = self.player.firstTile[1]

        self.pointTotals()



    ############ GAME STATE HANDLING ############

    def pointTotals(self) -> None:
        ''' Tallys up the total points that each player has
            and stores it in the Game State
        '''

        blackPoints = 0
        whitePoints = 0

        for row in self.gameBoard.board:
            
            for col in row:
                
                if col == 1:
                    blackPoints += 1
                
                elif col == 2:
                    whitePoints += 1

        self.points = blackPoints, whitePoints



    def checkGameState(self):
        self.checkGameOver()
        if not self.player.checkAvailableMoves(self):
            raise NoMovesAvailableError

    

    ############ END GAME HANDLING ############

    def _checkDraw(self) -> bool:
        ''' Checks to see if there is a tie in the Game State.
            If a Tie is found, the method returns True.
            Otherwise, False.
        '''

        if self.points[0] == self.points[1]:
            self._gameOver = None
            
            return True

        return False



    def checkGameOver(self) -> None:
        ''' Checks the current game state to see if the game has ended.
            if the game is over, a GameOverError is raised.
        '''

        if not self.player.checkAvailableMoves(self):
            self.player.nextTurn()
            
            if not self.player.checkAvailableMoves(self):
                self._gameOver = True
                
                raise GameOverError

            self.player.nextTurn()

        elif 0 in self.points:
            self._gameOver = True
            
            raise GameOverError

        else:
            self._gameOver = False



    def _winnerIs(self) -> str:
        ''' _winnerIs() determines the winner of a given GameState.
            If a tie is found, the winner is 'NONE'.  Otherwise
            the winner will be returned as 'B' or 'W'.
        '''

        self._checkDraw()
        if self._gameOver == None:
            return 'NONE'

        elif self.gameMode == '>':
            index = self.points.index(max(self.points))

        else:
            index = self.points.index(min(self.points))
            
        winner = index + 1

        return winner



class Player:
    ''' The Player object counts turns and handles all moves
        made by the players.  Although two players are required
        for the game, both players are represented by one object.
        Methods on the player object handle turn exchanges made by
        the two different players.
    '''

    def __init__(self, firstPlay, firstTile):
        ''' Determines who is the first player and who the upper
            left tile is granted to.
        '''

        self.turn = firstPlay
        self.firstTile = firstTile


    ############ MOVE VALIDATION HANDLING ############

    def _inRange(self, x, y, gameState) -> bool:
        ''' Checks to see if a hypothetical position exists
            on a GameBoard
        '''

        return (x in range(gameState.gameBoard.numRows) and
            y in range(gameState.gameBoard.numCols))



    def checkAvailableMoves(self, gameState: GameState) -> bool:
        ''' CheckAvailableMoves() scans the board for empty spaces
            and checks to see if these spaces are valid entries for
            moves based on the current GameState.

            Specifically it factors, whose turn it is, and the
            GameBoard in deciding what moves are valid for entry.
        '''

        for i in range(gameState.gameBoard.numRows):
            
            for j in range(gameState.gameBoard.numCols):
                
                if gameState.gameBoard.board[i][j] == 0:
                    move = [i, j]
                    
                    if self.checkMove(gameState, move):
                        return True

        return False



    def _checkDirection(self, gameState: GameState, 
        direction: list, move) -> bool:
        ''' Given a specific direction and a valid move, 
            _checkDirection checks to see if a specified
            direction has valid tiles to be captured by
            an opponent.  If tiles can be captured
            in that direction, then it is a valid move.
        '''

        newX = move[0] + direction[0]
        newY = move[1] + direction[1]

        board = gameState.gameBoard.board
        turn = gameState.player.turn

        if self._inRange(newX, newY, gameState):
            
            if board[newX][newY] not in [0, turn]:
                
                while True:
                    newX += direction[0]
                    newY += direction[1]
                    
                    if self._inRange(newX, newY, gameState):
                        
                        if board[newX][newY] == 0:
                            return False

                        elif board[newX][newY] == turn:
                            return True
                    
                    else:
                        return False



    def checkMove(self, gameState: GameState, move) -> bool:
        ''' checkMove() Checks for valid moves.

            Valid Move criteria are as follows:

                1. The specified tile cannot be already occupied
                2. There is at least one (1) direction that has
                    tiles that can be "captured"
            
        '''

        directions = [[0, 1], [1, 1], [1, 0], [1, -1],
                      [0, -1], [-1, -1], [-1, 0], [-1, 1]]

        if (not self._inRange(move[0], move[1], gameState) 
                or gameState.gameBoard.board[move[0]][move[1]] != 0):
            return False

        for direction in directions:

            if self._checkDirection(gameState, direction, move):
                return True
        
        return False


    ############ MOVE EXECUTION HANDLING ############

    def makeMove(self, gameState: GameState, move) -> list:
        ''' makeMove() executes a move by placing a tile on the board
            and capturing opponents tiles.  The point total is
            recalculated and the next player begins their turn.

            Coordinates of captured tiles are returned.
        '''

        x = move[0]
        y = move[1]

        gameState.gameBoard.board[x][y] = gameState.player.turn
        captured = gameState.player._capture(gameState, move)
        
        gameState.pointTotals()
        gameState.player.nextTurn()

        return captured



    def _capture(self, gameState: GameState, move) -> list:
        ''' _capture() looks in each direction and checks to see
            if a move in a specified direction is valid.  If it is,
            captures are taken into place.  The current player takes
            over their opponents tiles.  Additionally, coordinates for
            the captured tiles are returned
        '''

        directions = [[0, 1], [1, 1], [1, 0], [1, -1],
                      [0, -1], [-1, -1], [-1, 0], [-1, 1]]

        turn = gameState.player.turn
        board = gameState.gameBoard.board
        tilesCaptured = []          

        for direction in directions:
            x = move[0]
            y = move[1]

            if self._checkDirection(gameState, direction, move):

                while True:

                    x += direction[0]
                    y += direction[1]

                    if not self._inRange(x, y, gameState):
                        break

                    if board[x][y] == turn:
                        break

                    if board[x][y] not in [0, turn]:
                        board[x][y] = turn
                        tilesCaptured.append((x,y))

        return tilesCaptured



    def nextTurn(self) -> None:
        ''' nextTurn() handles turns for the Player object.
            If turn is 1, next turn is 2, and vice versa.
        '''
        if self.turn == 1:
            self.turn = 2

        else:
            self.turn = 1

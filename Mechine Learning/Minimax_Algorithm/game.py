import random
import copy
import numpy as np

class TeekoPlayer:
    """ An object representation for an AI game player for the game Teeko.
    """
    board = [[' ' for j in range(5)] for i in range(5)]
    pieces = ['b', 'r']

    def Position(self,state):
        b = []
        r = []
        for row in range(5):
            for col in range(5):
                if state[row][col] == 'b':
                    b.append((row,col))
                elif state[row][col] == 'r':
                    r.append((row,col))
        return b,r
    
    def __init__(self):
        """ Initializes a TeekoPlayer object by randomly selecting red or black as its
        piece color.
        """
        self.my_piece = random.choice(self.pieces)
        self.opp = self.pieces[0] if self.my_piece == self.pieces[1] else self.pieces[1]

    def heuristic_game_value(self, state, piece): # check largest number of pieces connected
        b,r = self.Position(state)
        if piece == 'b':
            mine = 'b'
            oppo = 'r'

        elif piece == 'r':
            mine = 'r'
            oppo = 'b'

        # for horizontal
        m_max = 0
        o_max = 0
        m_cnt = 0
        o_cnt = 0

        for i in range(len(state)):
            for j in range(len(state)):
                if state[i][j] == mine:
                    m_cnt += 1
            if m_cnt > m_max:
                m_max += m_cnt
            m_cnt = 0
        i=0
        j=0
        for i in range(len(state)):
            for j in range(len(state)):
                if state[i][j] == oppo:
                    o_cnt += 1
            if o_cnt > o_max:
                o_max = o_cnt
            o_cnt = 0

        # for vertical
        for i in range(len(state)):
            for j in range(len(state)):
                if state[j][i] == mine:
                    m_cnt += 1
            if m_cnt > m_max:
                m_max += m_cnt
            m_cnt = 0
        i=0
        j=0
        for i in range(len(state)):
            for j in range(len(state)):
                if state[j][i] == oppo:
                    o_cnt += 1
            if o_cnt > o_max:
                o_max += o_cnt
            o_cnt = 0


        # for / diagonal
        m_cnt = 0
        o_cnt = 0

        for row in range(3, 5):
            for i in range(2):
                if state[row][i] == mine:
                    m_cnt += 1
                if state[row - 1][i + 1] == mine:
                    m_cnt += 1
                if state[row - 2][i + 2] == mine:
                    m_cnt += 1
                if state[row - 3][i + 3] == mine:
                    m_cnt += 1

                if m_cnt > m_max:
                    m_max += m_cnt
                m_cnt = 0

        row = 0
        i= 0

        for row in range(3, 5):
            for i in range(2):
                if state[row][i] == oppo:
                    o_cnt += 1
                if state[row - 1][i + 1] == oppo:
                    o_cnt += 1
                if state[row - 2][i + 2] == oppo:
                    o_cnt += 1
                if state[row - 3][i + 3] == oppo:
                    o_cnt += 1
                if o_cnt > o_max:
                    o_max += o_cnt
                o_cnt = 0

        # for \ diagonal
        m_cnt = 0
        o_cnt = 0
        row = 0
        i = 0
        for row in range(2):
            for i in range(2):
                if state[row][i] == mine:
                    m_cnt += 1
                if state[row + 1][i + 1] == mine:
                    m_cnt += 1
                if state[row + 2][i + 2] == mine:
                    m_cnt += 1
                if state[row + 3][i + 3] == mine:
                    m_cnt += 1
                if m_cnt > m_max:
                    m_max += m_cnt
                m_cnt = 0

        row = 0
        i = 0
        for row in range(2):
            for i in range(2):
                if state[row][i] == oppo:
                    o_cnt += 1
                if state[row + 1][i + 1] == oppo:
                    o_cnt += 1
                if state[row + 2][i + 2] == oppo:
                    o_cnt += 1
                if state[row + 3][i + 3] == oppo:
                    o_cnt += 1
                if o_cnt > o_max:
                    o_max = o_cnt
                o_cnt = 0

        # for 2X2
        m_cnt = 0
        o_cnt = 0
        row = 0
        i = 0
        for row in range(4):
            for i in range(4):
                if state[row][i] == mine:
                    m_cnt += 1
                if state[row][i + 1] == mine:
                    m_cnt += 1
                if state[row + 1][i] == mine:
                    m_cnt += 1
                if state[row + 1][i + 1]== mine:
                    m_cnt += 1
                if m_cnt > m_max:
                    m_max += m_cnt
                m_cnt = 0

        row = 0
        i = 0
        for row in range(4):
            for i in range(4):
                if state[row][i] == oppo:
                    o_cnt += 1
                if state[row][i + 1] == oppo:
                    o_cnt += 1
                if state[row + 1][i] == oppo:
                    o_cnt += 1
                if state[row + 1][i + 1]== oppo:
                    o_cnt += 1
                if o_cnt > o_max:
                    o_max = o_cnt
                o_cnt = 0

        if m_max == o_max:
            return 0, state
        if m_max >= o_max:
            return m_max/5, state # if mine is longer than opponent, return positive float

        return (-1) * o_max/5, state # if opponent is longer than mine, return negative float

    def Max_Value(self, state, depth):
        bstate = state
        if self.game_value(state) != 0:
            return self.game_value(state),state

        if depth >= 2:
            return self.heuristic_game_value(state,self.my_piece)

        else:
            a = float('-Inf')
            for s in self.succ(state, self.my_piece):
                val = self.Min_Value(s,depth+1)
                if val[0] > a:
                    a = val[0]
                    bstate = s
        return a, bstate

    def Min_Value(self, state,depth):
        bstate = state
        if self.game_value(state) != 0:
            return self.game_value(state),state
        
        if depth >= 2:
            return self.heuristic_game_value(state, self.opp)

        else:
            b = float('Inf')
            for s in self.succ(state, self.opp):
                val = self.Max_Value(s,depth+1)
                if val[0] < b:
                    b = val[0]
                    bstate = s
        return b, bstate

    def succ(self, state, piece):
        self.game_value(state)
        mine = piece
        succ = list()
        drop_phase = True  # TODO: detect drop phase

        numB = sum((i.count('b') for i in state))
        numR = sum((i.count('r') for i in state))
        if numB >= 4 and numR >= 4:
            drop_phase = False

        if not drop_phase:
            for row in range(len(state)):
                for col in range(len(state)):
                    if state[row][col] == piece:
                        succ.append(self.up(state, row, col)) # (row-1)(col)
                        succ.append(self.down(state, row, col)) # (row+1)(col)
                        succ.append(self.left(state, row, col)) # (row)(col-1)
                        succ.append(self.right(state, row, col)) #
                        succ.append(self.upleft(state, row, col))
                        succ.append(self.upright(state, row, col))
                        succ.append(self.downleft(state, row, col))
                        succ.append(self.downright(state, row, col))
            return list(filter(None, succ))

        for row in range(len(state)):
            for col in range(len(state)):
                temp = copy.deepcopy(state)
                if temp[row][col] == ' ':
                    temp[row][col] = piece
                    succ.append(temp)
        return list(filter(None, succ))

    def up(self, k, i, j):
        state = copy.deepcopy(k)
        if i - 1 >= 0 and state[i - 1][j] == ' ':
            state[i][j], state[i - 1][j] = state[i - 1][j], state[i][j]
            return state

    def down(self, k, i, j):
        state = copy.deepcopy(k)
        if i + 1 < len(state) and state[i + 1][j] == ' ':
            state[i][j], state[i + 1][j] = state[i + 1][j], state[i][j]
            return state

    def left(self, k, i, j):
        state = copy.deepcopy(k)
        if j - 1 >= 0 and state[i][j - 1] == ' ':
            state[i][j], state[i][j - 1] = state[i][j - 1], state[i][j]
            return state

    def right(self, k, i, j):
        state = copy.deepcopy(k)
        if j + 1 < len(state) and state[i][j + 1] == ' ':
            state[i][j], state[i][j + 1] = state[i][j + 1], state[i][j]
            return state

    def upleft(self, k, i, j):
        state = copy.deepcopy(k)
        if i - 1 >= 0 and j - 1 >= 0 and state[i - 1][j - 1] == ' ':
            state[i][j], state[i - 1][j - 1] = state[i - 1][j - 1], state[i][j]
            return state

    def upright(self, k, i, j):
        state = copy.deepcopy(k)
        if i - 1 >= 0 and j + 1 < len(state) and state[i - 1][j + 1] == ' ':
            state[i][j], state[i - 1][j + 1] = state[i - 1][j + 1], state[i][j]
            return state

    def downleft(self, k, i, j):
        state = copy.deepcopy(k)
        if i + 1 < len(state) and j - 1 >= 0 and state[i + 1][j - 1] == ' ':
            state[i][j], state[i + 1][j - 1] = state[i + 1][j - 1], state[i][j]
            return state

    def downright(self, k, i, j):
        state = copy.deepcopy(k)
        if i + 1 < len(state) and j + 1 < len(state) and state[i + 1][j + 1] == ' ':
            state[i][j], state[i + 1][j + 1] = state[i + 1][j + 1], state[i][j]
            return state
                        

    def make_move(self, state):
        """ Selects a (row, col) space for the next move. You may assume that whenever
        this function is called, it is this player's turn to move.

        Args:
            state (list of lists): should be the current state of the game as saved in
                this TeekoPlayer object. Note that this is NOT assumed to be a copy of
                the game state and should NOT be modified within this method (use
                place_piece() instead). Any modifications (e.g. to generate successors)
                should be done on a deep copy of the state.

                In the "drop phase", the state will contain less than 8 elements which
                are not ' ' (a single space character).

        Return:
            move (list): a list of move tuples such that its format is
                    [(row, col), (source_row, source_col)]
                where the (row, col) tuple is the location to place a piece and the
                optional (source_row, source_col) tuple contains the location of the
                piece the AI plans to relocate (for moves after the drop phase). In
                the drop phase, this list should contain ONLY THE FIRST tuple.

        Note that without drop phase behavior, the AI will just keep placing new markers
            and will eventually take over the board. This is not a valid strategy and
            will earn you no points.
        """
        drop_phase = True
        numB = sum((i.count('b') for i in state))
        numR = sum((i.count('r') for i in state))
        if numB >= 4 and numR >= 4:
            drop_phase = False

        if not drop_phase:
            # TODO: choose a piece to move and remove it from the board
            # (You may move this condition anywhere, just be sure to handle it)
            #
            # Until this part is implemented and the move list is updated
            # accordingly, the AI will not follow the rules after the drop phase!
            move = list()
            value, bstate = self.Max_Value(state, 0)
            arr1 = np.array(state) == np.array(bstate)
            arr2 = np.where(arr1 == False) # check difference between succ and curr state
            if state[arr2[0][0]][arr2[1][0]] == ' ': # find original to define move
                (origrow, origcol) = (arr2[0][1].item(),arr2[1][1].item())
                (row,col) = (arr2[0][0].item(), arr2[1][0].item())
            else:
                (origrow, origcol) = (arr2[0][0].item(), arr2[1][0].item())
                (row, col) = (arr2[0][1].item(), arr2[1][1].item())
            move.append((row, col)) # move for after drop phase
            move.append((origrow, origcol))
            return move

        # select an unoccupied space randomly
        # TODO: implement a minimax algorithm to play better
        move = list()
        value, bstate = self.Max_Value(state,0)
        arr1 = np.array(state) == np.array(bstate)
        arr2 = np.where(arr1 == False) # check difference between succ and curr state
        (row,col) = (arr2[0][0].item(), arr2[1][0].item())
        while not state[row][col] == ' ': # find original to define move
            (row, col) = (arr2[0][0].item(), arr2[1][0].item())

        move.append((row, col))  # move for drop phase
        return move

    def opponent_move(self, move):
        """ Validates the opponent's next move against the internal board representation.
        You don't need to touch this code.

        Args:
            move (list): a list of move tuples such that its format is
                    [(row, col), (source_row, source_col)]
                where the (row, col) tuple is the location to place a piece and the
                optional (source_row, source_col) tuple contains the location of the
                piece the AI plans to relocate (for moves after the drop phase). In
                the drop phase, this list should contain ONLY THE FIRST tuple.
        """
        # validate input
        if len(move) > 1:
            source_row = move[1][0]
            source_col = move[1][1]
            if source_row != None and self.board[source_row][source_col] != self.opp:
                self.print_board()
                print(move)
                raise Exception("You don't have a piece there!")
            if abs(source_row - move[0][0]) > 1 or abs(source_col - move[0][1]) > 1:
                self.print_board()
                print(move)
                raise Exception('Illegal move: Can only move to an adjacent space')
        if self.board[move[0][0]][move[0][1]] != ' ':
            raise Exception("Illegal move detected")
        # make move
        self.place_piece(move, self.opp)

    def place_piece(self, move, piece):
        """ Modifies the board representation using the specified move and piece

        Args:
            move (list): a list of move tuples such that its format is
                    [(row, col), (source_row, source_col)]
                where the (row, col) tuple is the location to place a piece and the
                optional (source_row, source_col) tuple contains the location of the
                piece the AI plans to relocate (for moves after the drop phase). In
                the drop phase, this list should contain ONLY THE FIRST tuple.

                This argument is assumed to have been validated before this method
                is called.
            piece (str): the piece ('b' or 'r') to place on the board
        """
        if len(move) > 1:
            self.board[move[1][0]][move[1][1]] = ' '
        self.board[move[0][0]][move[0][1]] = piece

    def print_board(self):
        """ Formatted printing for the board """
        for row in range(len(self.board)):
            line = str(row)+": "
            for cell in self.board[row]:
                line += cell + " "
            print(line)
        print("   A B C D E")

    def game_value(self, state):
        """ Checks the current board status for a win condition

        Args:
        state (list of lists): either the current state of the game as saved in
            this TeekoPlayer object, or a generated successor state.

        Returns:
            int: 1 if this TeekoPlayer wins, -1 if the opponent wins, 0 if no winner

        TODO: complete checks for diagonal and box wins
        """
        # check horizontal wins
        for row in state:
            for i in range(2):
                if row[i] != ' ' and row[i] == row[i+1] == row[i+2] == row[i+3]:
                    return 1 if row[i]==self.my_piece else -1

        # check vertical wins
        for col in range(5):
            for i in range(2):
                if state[i][col] != ' ' and state[i][col] == state[i+1][col] == state[i+2][col] == state[i+3][col]:
                    return 1 if state[i][col]==self.my_piece else -1

        for i in range(2):
            for col in range(2):
                if state[i][col] != ' ' and state[i][col] == state[i+1][col+1] == state[i+2][col+2] == state[i+3][col+3]:
                    return 1 if state[i][col]==self.my_piece else -1
        
        for i in range(3, 5):
            for col in range(2):
                if state[i][col] != ' ' and state[i][col] == state[i-1][col+1] == state[i-2][col+2] == state[i-3][col+3]:
                    return 1 if state[i][col]==self.my_piece else -1

        for i in range(4):
            for col in range(4):
                if state[i][col] != ' ' and state[i][col] == state[i][col+1] == state[i+1][col] == state[i+1][col+1]:
                    return 1 if state[i][col]==self.my_piece else -1

        return 0 # no winner yet

############################################################################
#
# THE FOLLOWING CODE IS FOR SAMPLE GAMEPLAY ONLY
#
############################################################################
def main():
    print('Hello, this is Samaritan')
    ai = TeekoPlayer()
    piece_count = 0
    turn = 0
    # drop phase
    while piece_count < 8 and ai.game_value(ai.board) == 0:

        # get the player or AI's move
        if ai.my_piece == ai.pieces[turn]:
            ai.print_board()
            move = ai.make_move(ai.board)
            ai.place_piece(move, ai.my_piece)
            print(ai.my_piece+" moved at "+chr(move[0][1]+ord("A"))+str(move[0][0]))
        else:
            move_made = False
            ai.print_board()
            print(ai.opp+"'s turn")
            while not move_made:
                player_move = input("Move (e.g. B3): ")
                while player_move[0] not in "ABCDE" or player_move[1] not in "01234":
                    player_move = input("Move (e.g. B3): ")
                try:
                    ai.opponent_move([(int(player_move[1]), ord(player_move[0])-ord("A"))])
                    move_made = True
                except Exception as e:
                    print(e)

        # update the game variables
        piece_count += 1
        turn += 1
        turn %= 2

    # move phase - can't have a winner until all 8 pieces are on the board
    while ai.game_value(ai.board) == 0:

        # get the player or AI's move
        if ai.my_piece == ai.pieces[turn]:
            ai.print_board()
            move = ai.make_move(ai.board)
            ai.place_piece(move, ai.my_piece)
            print(ai.my_piece+" moved from "+chr(move[1][1]+ord("A"))+str(move[1][0]))
            print("  to "+chr(move[0][1]+ord("A"))+str(move[0][0]))
        else:
            move_made = False
            ai.print_board()
            print(ai.opp+"'s turn")
            while not move_made:
                move_from = input("Move from (e.g. B3): ")
                while move_from[0] not in "ABCDE" or move_from[1] not in "01234":
                    move_from = input("Move from (e.g. B3): ")
                move_to = input("Move to (e.g. B3): ")
                while move_to[0] not in "ABCDE" or move_to[1] not in "01234":
                    move_to = input("Move to (e.g. B3): ")
                try:
                    ai.opponent_move([(int(move_to[1]), ord(move_to[0])-ord("A")),
                                    (int(move_from[1]), ord(move_from[0])-ord("A"))])
                    move_made = True
                except Exception as e:
                    print(e)

        # update the game variables
        turn += 1
        turn %= 2

    ai.print_board()
    if ai.game_value(ai.board) == 1:
        print("AI wins! Game over.")
    else:
        print("You win! Game over.")


if __name__ == "__main__":
    main()

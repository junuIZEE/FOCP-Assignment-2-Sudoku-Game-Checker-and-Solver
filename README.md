Sudoku Game – Checker and Solver
Assignment #2 – FOCP (CS-110)
Names: Jyotish Kumar, Bilal Ghazi, Umayrah Masood Malik
Date: 30/11/2025

> Overview

This is a console-based Sudoku game made in C++ which allows the user to play Sudoku in a 9x9 grid. The program also possesses additional features such as variable difficulty levels, hints, an Auto-solver and a scoring system to improve user experience. 


> How the Program Works
 
 
Design and Logic:
OVERVIEW
The design for the game is simple. The start involves asking the user for difficulty based on which a preset is made. Some values are also initialized (both together and beforehand) like score, start time, moves made, etc. These will be tracked and displayed at the end of the program. Once the preset and boards have been set (a separate solvedBoard exists onto which the preset is mapped, and the solve() function is called on solvedBoard. The program then goes into an infinite loop (while true), and continuously draws the main board, prompts for input (Insertion? Hint? Solve? Check?) and does action based on that input. Additionally, the loop also checks to see if the player asked for a solve or check (because those can be reasons to terminate the program) (Improvement 1 is relevant here).

FUNCTIONS (only the major functions worth discussing, the rest are easily understandable by looking at the code and comments):

1. "bool solve(int grid[9][9], int row, int column)":
The above function is a recursive function that takes a board (which it will modify and solve) as input, and additionally row and column to recursively go through it (when called, both given as 0). 
The core idea is it returns true if it solves it and false if it can’t. The base case for it is: Row is 9 (but since 0 indexed it means row 10 however the board is 9x9). It implies it went reached column 8 (9th in reality) of row 8 (also 9th in reality) meaning the end of the line and went forward another cell (which doesn’t exist) so it terminates.
Following the base case, it decides the next row and column it needs to go to. If it is at a preset value it moves on ahead to the next cell.
Then it falls into a loop of values ranging from 1 to 9 (inclusive). It checks if setting the current cell to i would be invalid. In the case it isn’t, it sets the cell to i, then calls itself as an if statement, that if the board can be solved now. Each time it calls itself it uses the same board as a parameter but gives next row and column as input.
At some point in it calling itself it will be unable to find a valid value, where it will then exit the loop (all values tried), set the value back to 0, and return false (therefore telling the solve that called it to not go down this path). If all can solve, then they will go back and return true through all the calls and this both solves the board and lets the first caller know it can be solved. 
 
2. "int fill_preset(int row, int column)":
Functions almost identically to solve() however instead of trying values in order it does them randomly.
It shuffles an array of values ranging from 1-9 inclusive and tries them in order instead. It fills the board similar to solve. The randomness is there to allow for many possible sudoku boards.
Additionally it could return bool, a minor improvement. 

3. "void count_solutions(int grid[9][9], int row, int column)":
This functions counts how many solutions exist for a sudoku puzzle. It doesn’t return any value and instead adjusts a global variable, solutions.
Its base case is that row is 9, meaning all 81 cells filled successfully, It implies one solution so global counter incremented and it returns.
It calculates next cell same as in previous functions, skips presets, but importantly, tries all possibilities for each empty cell. If a move is valid it recursively explores that path, but, it doesn’t return. Then resets the cell to 0 before moving onto the next cell. 
This function however also modifies the board and therefore an intermediary function was made “get_solutions” that passes on a clone to count_solutions and also returns the solutions.

4. "void gap_preset(int difficulty)":
Once a preset has been made, this function introduces gaps in the preset so that there is a puzzle for the player to solve at all. Our attempt at introducing difficulty involved creating holes (though this is only one of the ways going about this) proportionate to the difficulty selected.
One thing to note is that creating holes in a sudoku puzzle CAN result in multiple solutions however the gap_preset function ensures one unique solution with each gap it tries to make (Sudoku require a unique solution). That however can prevent there being holes exactly proportional to the difficulty selected. The goal is to make 16 * difficulty selected holes, so if 5 is the difficulty there would be 80 holes however it is not going to reach that amount. A 9x9 sudoku grid has 81 cells and 80 holes would imply there would be exactly 1 cell filled. Just one filled cell allows for many, many solutions. Therefore, the result is it being as difficult as can be (within constraints given).
It iterates from 0 to difficulty * 16, and it randomly picks a cell that isn’t empty (a hole already). It makes it 0 and if it returns multiple solutions, it sets the value back to what it was prior to erasure and makes 10 attempts at find an alternative, and if it still can’t find any it lets the cell be and moves on. (Improvement 2 is relevant here)

Possible Improvements:
1.	Instead of having redundant if statements, the while loop could simply be while (!IS_SOLVED) and IS_SOLVED be adjusted by the solve and check cases in the switch statements in action() and loop, removing the need for return statements.
2.	Alongside gaps, difficulty could be implemented in more ways than just gaps, who themselves are being compromised in the current code.

 
Prominent Features:

• Sudoku board generator: 
 The program generates a solvable Sudoku board.

• Hint system: 
The user can request hints to aid in solving the board.

• Auto-solver: 
The user can opt to automatically solve the board.

• Score system: 
A scoring system evaluates the users efficiency based on correct moves, mistakes, and hint usage.
• Clean output: 
The display features easily distinguishable colors that improve user experience.

• Difficulty system: 
The user can choose the difficulty level of the board from 1-5; at higher difficulty levels the board starts with less pre-filled spaces.

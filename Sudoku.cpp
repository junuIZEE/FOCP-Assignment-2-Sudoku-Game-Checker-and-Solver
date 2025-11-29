// Headers
#include <iostream>
#include <Windows.h>
#include <string>
#include <algorithm>

using namespace std;

//			Definitions
// Scoring Mechanism
#define VALID 10
#define INVALID -5
#define HINT -10
#define SOLUTION -20
#define TIME -0.25

// Colors, for console output
#define white 7
#define gray 8
#define green 10
#define cyan 11
#define red 12
#define yellow 14

//			Prototypes
//		Logic

// Sets the boards i.e the preset and 
void set_boards(int difficulty);

// The logic for generating presets 
void generate_preset(int difficulty);

// Fills the presets with values (basically makes it)
int fill_preset(int row, int column);

// Introduces gaps in the preset to make for a puzzle (also ensures exactly one solution exists with each gap made)
// Difficulty roughly dictates how many gaps to make
void gap_preset(int difficulty);

// Returns char in lowercase
static inline char to_lower(char c) { if (c >= 'A' && c <= 'Z') return c + 'a' - 'A'; else return c; }

// Checks if a value repeats in its row or column
bool repeats_axially(int grid[9][9], int key, int row, int column);

// Checks if any values repeat axially
bool any_repeat_axially(int grid[9][9]);

// Checks if a value repeats in its 3x3 section
bool repeats_in_section(int grid[9][9], int key, int row, int column);

// Checks if any values repeat in their sections
bool any_repeat_sectionally(int grid[9][9]);

// Checks if a player's insert would overwrite a preset value
bool is_overwriting(int row, int column);

// Combines above logic to tell whetehr a move is valid
bool invalid(int grid[9][9], int key, int row, int column);

// Handles different actions
void action(string input);

// Write a value into the board
void insert(string input);

// Get a hint
void hint();

// Solve a board based on Preset
bool solve(int grid[9][9], int row, int column);

// Checks if the main board is solved
bool solved();

// Adjusts global SOLUTIONS, according to the number of solutions to a board
void count_solutions(int grid[9][9], int row, int column);

// Does the same as count_solutions however count_solutions modifies the boards so this acts as the intermediate, always giving a clone to the function based on what it is given
int get_solutions(int of[9][9]);

// Safely gets an int, within a range
int get_int(string prompt, int min, int max);

// Calculates the score based on the mechanism and plays made in the game
int calculate_score();

// Prompts the user for input
string prompt();

//		Visuals
// Draws the board in a easy to read manner
void draw_board(int grid[9][9]);

// Displays statistics, eg: Time taken, hints used, etc.
void display_stats();

// Prints text with color
static inline void printc(string text, int color) { HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); SetConsoleTextAttribute(hConsole, color); cout << text; SetConsoleTextAttribute(hConsole, 7); }

// Prints a lot of newlines to keep the interface clean
static inline void space() { cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"; }

//			Globals
int SCORE = 0;
int MOVES = 0;
int CORRECT = 0;
int WRONG = 0;
int HINTS = 0;

bool IS_SOLVED = false;

int START_TIME = time(0);

int solutions = 0;

int PRESET[9][9] = {
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
};

int board[9][9];

int solvedBoard[9][9];

//			Main
int main() {

	cout << "Hello player! Do you dare to solve a sudoku puzzle? [ENTER TO START]\n";

	cin.get();

	{ // Initializing
		srand(time(0));

		int difficulty;
		difficulty = get_int("Enter a difficulty (1-5): ", 1, 5);
		printc("\nGenerating a board...\n\n", white);
		set_boards(difficulty);

		solve(solvedBoard, 0, 0);

		// Instructions
		printc("TO  INSERT, TYPE:  <row><column><value, '0' to erase>\nFOR HINT,   TYPE:  h\nFOR CHECK,  TYPE:  c\nTO  SOLVE,  TYPE:  s\n\n", gray);
	
		SCORE = 0;
		START_TIME = time(0);
	}

	string input;

	while (true) {

		draw_board(board);

		string input = prompt();

		space();

		action(input);

		cout << endl;

		if (input[0] == 's') {	
			return 0;
		}
		else if (input[0] == 'c') {
			if (IS_SOLVED) return 0;
		}
	}
}

//			Helpers
void set_boards(int difficulty) {
	// First set preset

	// Generate the preset based on difficulty
	generate_preset(difficulty);

	// Once made, set both the board and solved board to preset.
	// solve() will be called on solvedBoard as well
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			board[i][j] = PRESET[i][j];
			solvedBoard[i][j] = PRESET[i][j];
		}
	}

}

void generate_preset(int difficulty) {
	
	// To give the solver something to work on
	int first_row[9] = { 1,2,3,4,5,6,7,8,9 };
	random_shuffle(first_row, first_row + 9);

	// Randomizes the first row
	for (int i = 0; i < 9; i++) {
		PRESET[0][i] = first_row[i];
	}

	// Solves the board
	solve(PRESET, 0, 0);

	// Then introduces gaps for the player to fill, while making sure there is exactly 1 solution to the puzzle.
	gap_preset(difficulty);
}

bool repeats_axially(int grid[9][9], int key, int row, int column) {

	if (key == 0) return false;

	// Check Horizontally
	for (int i = 0; i < 9; i++) {
		if (i != column && grid[row][i] == key) return true;
	}

	// Vertically
	for (int i = 0; i < 9; i++) {
		if (i != row && grid[i][column] == key) return true;
	}

	return false;
}

bool any_repeat_axially(int grid[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (repeats_axially(grid, grid[i][j], i, j)) return true;
		}
	}
	return false;
}

bool repeats_in_section(int grid[9][9], int key, int row, int column) {

	if (key == 0) return false;

	// Section
	int sec_x = (column) / 3;
	int sec_y = (row) / 3;

	for (int i = 3 * sec_y; i < 3 + (3 * sec_y); i++) {
		for (int j = 3 * sec_x; j < 3 + (3 * sec_x); j++) {
			if ((i != row && j != column) && grid[i][j] == key) return true;
		}
	}

	return false;
}

bool any_repeat_sectionally(int grid[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (repeats_in_section(grid, grid[i][j], i, j)) return true;
		}
	}
	return false;
}

bool invalid(int grid[9][9], int key, int row, int column) {
	// If a value repeats in row, repeats in its column, or if it isn't 1-9 it is invalid.
	return (repeats_in_section(grid, key, row, column) || repeats_axially(grid, key, row, column) || key < 0 || key > 9);
}

bool is_overwriting(int row, int column) {
	return (PRESET[row][column] == 0) ? false : true;
}

void action(string input) {
	if (input.length() == 3) { insert(input); return; }

	char act = input[0];

	switch (act) {
	case 'c':
		IS_SOLVED = solved();
		if (IS_SOLVED) {
			printc("Congratulations, You solved the board!\n\n", yellow);
			cout << endl;

			display_stats();
		}
		else printc("The board is not yet solved.\n", red);
		break;
	case 'h':
		hint();
		break;
	case 's':
		printc("Unfortunately, you could not solve the board.\nThis is the solved board:\n\n", yellow);
		draw_board(solvedBoard);
		cout << endl;

		SCORE += SOLUTION;

		display_stats();

		break;

	default:
		cout << "Invalid input\n" << endl;
		return;
	}
}

void insert(string input) {

	// Validate input
	if (input[0] < '1' || input[0] > '9') { cout << "\nRow must be a number.\n"; return; }
	if (!isalpha(input[1])) { cout << "\nColumn must be an alphabet.\n"; return; }
	if (input[2] < '0' || input[2] > '9') { cout << "\nValue must be a number.\n"; return; }

	int row = static_cast<int> (input[0]) - '1';
	int column = static_cast<int> (to_lower(input[1])) - 'a';
	int value = static_cast<int> (input[2]) - '1' + 1;

	if (value == board[row][column]) return;

	if (!is_overwriting(row, column)) {
		board[row][column] = value;
	}
	else {
		cout << "\n\nThis number is a preset number, you can't overwrite it.\n\n";
		return;
	}

	MOVES++;

	if (invalid(board, value, row, column)) {
		WRONG++;
	}
	else if (value != 0) {
		CORRECT++;
	}
}

void hint() {
	// Find an empty cell in main board
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (board[i][j] == 0 || board[i][j] != solvedBoard[i][j]) {
				board[i][j] = solvedBoard[i][j];
				
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), yellow);
				cout << "\nHint move has been played,\n";
				cout << solvedBoard[i][j] << " (value), " << i + 1 << " (row), " << static_cast<char>(j + 65) << " (column)\n";
				
				HINTS++;
				return;
			}
		}
	}
}

bool solved() {
	if (any_repeat_axially(board) || any_repeat_sectionally(board)) return false;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (board[i][j] == 0) return false;
		}
	}

	return true;
}

int fill_preset(int row, int column) {
	// Base case: reached end of board?
	if (row == 9) {
		return true;
	}

	int nextRow = (column == 8) ? row + 1 : row;
	int nextCol = (column == 8) ? 0 : column + 1;

	// If current cell is preset, skip to next cell
	if (is_overwriting(row, column)) {
		return fill_preset(nextRow, nextCol);
	}

	// Try random values 1-9, random so that the preset is different each play:
	int values[9] = { 1,2,3,4,5,6,7,8,9 };
	random_shuffle(values, values + 9);

	// 9 times
	for (int i = 0; i < 9; i++) {
		int val = values[i];

		// If valid, place it
		if (!invalid(PRESET, val, row, column)) {
			PRESET[row][column] = val;

			if (fill_preset(nextRow, nextCol)) {
				return true;
			}
		}
	}

	// Reset values if can't fill
	PRESET[row][column] = 0;
	return false;
}

void gap_preset(int difficulty) {
	int row;
	int col;
	int val;

	for (int i = 0; i < difficulty * 16; i++) {
		solutions = 0;

		// Random row and column selected

		do {
			row = rand() % 9;
			col = rand() % 9;
			val = PRESET[row][col];
		} while (val == 0);

		PRESET[row][col] = 0;

		// Multiple solutions occurred.
		// Try 10 more times to find a cell to erase
		// Stop the loop the first time it can erase
		// If after 10 times it still can't find an erasable cell

		if (get_solutions(PRESET) != 1) {

			// Reset the value
			PRESET[row][col] = val;

			int stuckRow = row;
			int stuckCol = col;

			for (int j = 0; j < 10; j++) {
				do { // A position other than the current one that isn't 0.

					row = rand() % 9;
					col = rand() % 9;

					val = PRESET[row][col];
				} while (val == 0 || ((row == stuckRow && col == stuckCol)));
				PRESET[row][col] = 0;

				solutions = 0;

				// Return the value and let it be
				if (get_solutions(PRESET) == 1) break;  // Found good cell, stop
				else PRESET[row][col] = val;  // Didn't work, restore
			}
		}

	}

}

bool solve(int grid[9][9], int row, int column) {
	// Base case: reached end of board?
	if (row == 9) {
		return true;
	}
	int nextRow = (column == 8) ? row + 1 : row;
	int nextCol = (column == 8) ? 0 : column + 1;
	// If current cell is preset, skip to next cell
	if (is_overwriting(row, column)) {
		return solve(grid, nextRow, nextCol);
	}
	// Try values 1-9:
	for (int i = 1; i <= 9; i++) {
		// If valid, place it
		if (!invalid(grid, i, row, column)) {
			grid[row][column] = i;
			// Recursively solve next cell,
			// If recursive call succeeds, return success.
			// Else, if it fails, undo (set back to 0) and try next value
			if (solve(grid, nextRow, nextCol)) {
				return true;
			}
		}
	}
	// If all values 1-9 fail, return failure (triggers backtracking)
	grid[row][column] = 0;
	return false;
}

void count_solutions(int grid[9][9], int row, int column) {

	// Base case: reached end of board? One solution here
	if (row == 9) {
		solutions++;
		return;
	}

	int nextRow = (column == 8) ? row + 1 : row;
	int nextCol = (column == 8) ? 0 : column + 1;

	if (grid[row][column] != 0) {
		count_solutions(grid, nextRow, nextCol);
		return;
	}

	// Try values 1-9
	for (int i = 1; i <= 9; i++) {

		// If valid, place it
		if (!invalid(grid, i, row, column)) {
			grid[row][column] = i;
			count_solutions(grid, nextRow, nextCol);  // Don't return, keep going
			grid[row][column] = 0;  // Backtrack to try next value
		}
	}
}

int get_solutions(int of[9][9]) {
	int clone[9][9];

	// Creates a clone to give to count_solutions()
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			clone[i][j] = of[i][j];
		}
	}

	solutions = 0;

	count_solutions(clone, 0, 0);

	return solutions;
}

int calculate_score() {
	
	// Based on time, in seconds
	SCORE += TIME * (time(0) - START_TIME);

	// Hints
	SCORE += HINT * HINTS;

	// Wrong
	SCORE += WRONG * INVALID;

	// Right
	SCORE += CORRECT * VALID;

	return (SCORE > 0)? SCORE: 0;
}

void draw_board(int grid[9][9]) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	printc("\n    A B C   D E F   G H I\n", gray);
	printc("   -----------------------\n", gray);

	for (int i = 0; i < 9; i++) {
		printc({ static_cast<char>(i + 49), '\0' }, gray); printc(" | ", gray);
		for (int j = 0; j < 9; j++) {

			if (grid[i][j] != 0) {
				// Check if it's a preset number
				if (PRESET[i][j] != 0) {
					SetConsoleTextAttribute(hConsole, 11);  // Cyan for presets
				}
				else {
					SetConsoleTextAttribute(hConsole, 10);  // Green for user input
				}
				cout << grid[i][j];
				SetConsoleTextAttribute(hConsole, 7);  // Reset to white
				cout << ' ';
			}
			else {
				cout << "  ";
			}

			if (j % 3 == 2)
				printc("| ", gray);
		}
		cout << endl;
		if (i % 3 == 2)
			printc("   -----------------------\n", gray);
	}
}

int get_int(string prompt, int min, int max) {
	int n;

	do {
		cout << prompt << flush;

		if (!(cin >> n)) {
			printc("Invalid input.\n\n", red);
			cin.clear();
			cin.ignore(max + 1, '\n');
			continue;
		}

	} while (n < min || n > max);

	cin.ignore(max + 1, '\n');  // Add this line to consume the leftover newline

	return n;
}

string prompt() {	
	cout << "\nEnter your next action: ";

	string line;
	getline(cin, line);

	return line;
}

void display_stats() {
	SCORE = calculate_score();
	int elapsedTime = time(0) - START_TIME;
	int minutes = elapsedTime / 60;
	int seconds = elapsedTime - 60 * minutes;

	printc("FINAL STATS:", green);
	printc("====================\n", gray);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), yellow);

	
	cout << "Score:       " << SCORE << endl;
	cout << "Time taken:  " << minutes << " minutes, " << seconds << " seconds" << endl;
	cout << "Moves:       " << MOVES << endl;
	cout << "Hints used:  " << HINTS << endl;
	cout << "Right moves: " << CORRECT << endl;
	cout << "Wrong moves: " << WRONG << endl;
	printc("====================\n", gray);
}

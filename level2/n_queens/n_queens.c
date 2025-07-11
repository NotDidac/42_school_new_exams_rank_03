#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global variables to avoid passing many parameters
int *board;         // board[col] = row position of queen in column col
int board_size;     // size of the board (n)

// Check if placing a queen at (row, col) is safe
int is_safe(int row, int col)
{
    int i;
    
    // Check all previously placed queens (columns 0 to col-1)
    for (i = 0; i < col; i++)
    {
        // Check if queens are in same row
        if (board[i] == row)
            return 0;
        
        // Check diagonal attacks
        // If the difference in rows equals difference in columns,
        // they're on the same diagonal
        if (abs(board[i] - row) == abs(i - col))
            return 0;
    }
    
    return 1;
}

// Print the current solution
void print_solution(void)
{
    int i;
    
    for (i = 0; i < board_size; i++)
    {
        // Print row position of queen in column i
        fprintf(stdout, "%d", board[i]);
        
        // Add space between numbers, except after the last one
        if (i < board_size - 1)
            write(1, " ", 1);
    }
    write(1, "\n", 1);
}

// Backtracking function to solve n-queens
void solve_nqueens(int col)
{
    int row;
    
    // Base case: if we've placed queens in all columns
    if (col == board_size)
    {
        print_solution();
        return;
    }
    
    // Try placing queen in each row of current column
    for (row = 0; row < board_size; row++)
    {
        // Check if this position is safe
        if (is_safe(row, col))
        {
            // Place queen at this position
            board[col] = row;
            
            // Recursively solve for next column
            solve_nqueens(col + 1);
            
            // Backtrack (remove queen) - implicit since we overwrite
            // board[col] in next iteration or when function returns
        }
    }
}

int main(int argc, char **argv)
{
    int n;
    
    // Check arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }
    
    // Parse board size
    n = atoi(argv[1]);
    
    // Handle edge cases
    if (n <= 0)
        return 0;
    
    // No solution exists for n=2 and n=3
    if (n == 2 || n == 3)
        return 0;
    
    // Set global variables
    board_size = n;
    
    // Allocate board array
    board = malloc(n * sizeof(int));
    if (!board)
        return 1;
    
    // Start solving from column 0
    solve_nqueens(0);
    
    // Clean up
    free(board);
    
    return 0;
}

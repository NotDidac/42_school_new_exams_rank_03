#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
char *input_string;
int string_length;
char **all_permutations;
int perm_count;
int perm_capacity;

// Function to swap two characters
void swap_chars(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Function to compare two strings (returns -1, 0, or 1)
int string_compare(char *s1, char *s2)
{
    int i = 0;
    
    while (s1[i] && s2[i])
    {
        if (s1[i] < s2[i])
            return -1;
        if (s1[i] > s2[i])
            return 1;
        i++;
    }
    
    // Handle different lengths
    if (s1[i] && !s2[i])
        return 1;
    if (!s1[i] && s2[i])
        return -1;
    
    return 0;
}

// Function to add a permutation to our collection
void add_permutation(char *perm)
{
    char *new_perm;
    int i;
    
    // Allocate space for new permutation
    new_perm = malloc((string_length + 1) * sizeof(char));
    if (!new_perm)
        exit(1);
    
    // Copy the permutation
    for (i = 0; i <= string_length; i++)
        new_perm[i] = perm[i];
    
    // Expand array if needed
    if (perm_count >= perm_capacity)
    {
        perm_capacity *= 2;
        all_permutations = realloc(all_permutations, perm_capacity * sizeof(char *));
        if (!all_permutations)
            exit(1);
    }
    
    // Add to collection
    all_permutations[perm_count] = new_perm;
    perm_count++;
}

// Recursive function to generate all permutations
void generate_permutations(char *str, int left, int right)
{
    int i;
    
    // Base case: if left == right, we have a complete permutation
    if (left == right)
    {
        add_permutation(str);
        return;
    }
    
    // Try each character at the current position
    for (i = left; i <= right; i++)
    {
        // Swap current character with the character at 'left'
        swap_chars(&str[left], &str[i]);
        
        // Recursively generate permutations for the rest
        generate_permutations(str, left + 1, right);
        
        // Backtrack: swap back to restore original order
        swap_chars(&str[left], &str[i]);
    }
}

// Function to sort all permutations alphabetically using bubble sort
void sort_permutations(void)
{
    int i, j;
    char *temp;
    
    // Bubble sort
    for (i = 0; i < perm_count - 1; i++)
    {
        for (j = 0; j < perm_count - 1 - i; j++)
        {
            if (string_compare(all_permutations[j], all_permutations[j + 1]) > 0)
            {
                // Swap pointers
                temp = all_permutations[j];
                all_permutations[j] = all_permutations[j + 1];
                all_permutations[j + 1] = temp;
            }
        }
    }
}

// Function to print all permutations
void print_permutations(void)
{
    int i;
    
    for (i = 0; i < perm_count; i++)
    {
        puts(all_permutations[i]);
    }
}

// Function to free all allocated memory
void free_permutations(void)
{
    int i;
    
    for (i = 0; i < perm_count; i++)
    {
        free(all_permutations[i]);
    }
    free(all_permutations);
}

int main(int argc, char **argv)
{
    char *working_string;
    int i;
    
    // Check arguments
    if (argc != 2)
    {
        return 1;
    }
    
    // Get input string and its length
    input_string = argv[1];
    string_length = strlen(input_string);
    
    // Handle empty string
    if (string_length == 0)
    {
        puts("");
        return 0;
    }
    
    // Initialize permutation storage
    perm_capacity = 120; // Start with reasonable capacity (5! = 120)
    all_permutations = malloc(perm_capacity * sizeof(char *));
    if (!all_permutations)
        exit(1);
    
    perm_count = 0;
    
    // Create working copy of the string
    working_string = malloc((string_length + 1) * sizeof(char));
    if (!working_string)
        exit(1);
    
    for (i = 0; i <= string_length; i++)
        working_string[i] = input_string[i];
    
    // Generate all permutations
    generate_permutations(working_string, 0, string_length - 1);
    
    // Sort permutations alphabetically
    sort_permutations();
    
    // Print all permutations
    print_permutations();
    
    // Clean up
    free(working_string);
    free_permutations();
    
    return 0;
}

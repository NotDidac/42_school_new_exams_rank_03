#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <float.h>

// compute the distance between two points
float    distance(float a[2], float b[2])
{
    return sqrtf((b[0] - a[0]) * (b[0] - a[0]) + (b[1] - a[1]) * (b[1] - a[1]));
}

/* YOUR FUNCTIONS START HERE */
/*Helper function to generate permutations of cities and 
calculate the total path length for each.
It uses a recursive backtracking approach.
cities: The array of all city coordinates.
num_cities: The total number of cities to visit.
current_path: An array storing the indices of cities 
	in the current permutation being built.
visited: A boolean array to keep track of which cities 
	have already been included in current_path for the current permutation.
current_dist_sum: The accumulated distance of the path segments formed so far 
	in the current permutation.
level: The current depth of recursion, indicating which position 
	in current_path we are currently trying to fill.
best_distance_ptr: A pointer to a float variable that stores 
	the minimum distance found across all permutations.
start_node_index: The index of the city that is fixed as the starting point 
	of the path to avoid redundant calculations
    (since the total length of a cycle is independent of its starting point).*/
void permute(float (*cities)[2], ssize_t num_cities, int *current_path, bool *visited,
             float current_dist_sum, int level, float *best_distance_ptr, int start_node_index)
{
    // Base case: If all cities have been placed in the current_path permutation.
    if (level == num_cities)
    {
        // The path is now complete. To close the loop, add the distance from the last city
        // in the permutation back to the fixed starting city.
        current_dist_sum += distance(cities[current_path[num_cities - 1]], cities[start_node_index]);

        // If the total distance of this path is less than the best distance found so far, update best_distance_ptr.
        if (current_dist_sum < *best_distance_ptr)
        {
            *best_distance_ptr = current_dist_sum;
        }
        return; // Backtrack
    }

    // Recursive step: Iterate through all cities to find the next one to add to the path.
    for (int i = 0; i < num_cities; i++)
    {
        // If city 'i' has not been visited in the current permutation.
        if (!visited[i])
        {
            // Mark city 'i' as visited for this branch of the recursion.
            visited[i] = true;
            // Place city 'i' at the current level in the current_path.
            current_path[level] = i;

            // Calculate the distance added by moving to this new city.
            float distance_to_add = 0.0f;
            // If this is not the very first city being placed (i.e., not the one fixed at level 0),
            // calculate the distance from the previous city in the path to the current one.
            if (level > 0)
            {
                distance_to_add = distance(cities[current_path[level - 1]], cities[current_path[level]]);
            }
            // Note: If level is 0, distance_to_add remains 0.0f. The distance from the fixed start_node_index
            // to current_path[0] (which is the first city chosen in the permutation) will be implicitly
            // handled when level becomes 1, as current_path[0] will be the "previous" city.

            // Recursively call permute to fill the next level of the path.
            permute(cities, num_cities, current_path, visited,
                    current_dist_sum + distance_to_add, level + 1, best_distance_ptr, start_node_index);

            // Backtrack: Mark city 'i' as unvisited so it can be used in other permutations.
            visited[i] = false;
        }
    }
}

// Main function to solve the Traveling Salesman Problem.
// (the skeleton has been provided, you need to fill in the blanks.)
// array: A pointer to an array of city coordinates ([x, y] pairs).
// size: The number of cities in the array.
// Returns the length of the shortest possible closed path visiting all cities.
float tsp(float (*array)[2], ssize_t size)
{
    float best_distance;
    
	
	// ... YOUR CODE STARTS HERE 

	
	best_distance = FLT_MAX;
	 // Handle edge cases for 0 or 1 city: the path length is 0.
    if (size <= 1) {
        return 0.0f;
    }

    // Allocate memory for the current_path (stores city indices in a permutation)
    // and visited (tracks visited cities) arrays.
    int *current_path = (int *)calloc(size, sizeof(int));
    bool *visited = (bool *)calloc(size, sizeof(bool));

    // Check if memory allocation was successful.
    if (!current_path || !visited) {
        fprintf(stderr, "Error: Memory allocation failed in tsp.\n");
        // Free any successfully allocated memory before returning.
        free(current_path);
        free(visited);
        return FLT_MAX; // Return a large value to indicate an error.
    }

    // Fix the first city (city at index 0) as the starting point.
    // This reduces the number of permutations to (N-1)! instead of N!,
    // as the total path length of a cycle is independent of its starting point.
    int start_node_index = 0;
    current_path[0] = start_node_index;
    visited[start_node_index] = true;

    // Start the permutation generation from the second city (level 1).
    // The initial current_dist_sum is 0.0f, as the first segment's distance
    // (from the fixed start node to the first chosen city in the permutation)
    // will be added in the first recursive call of permute.
    permute(array, size, current_path, visited, 0.0f, 1, &best_distance, start_node_index);

    // Free the dynamically allocated memory.
    free(current_path);
    free(visited);

	// ... YOUR CODE ENDS 


    return (best_distance);
}


/* YOUR FUNCTIONS END HERE */

// Function to determine the number of lines (cities) in a file.
// file: A pointer to the FILE stream.
// Returns the number of lines, or -1 on error.
ssize_t    file_size(FILE *file)
{
    char    *buffer = NULL;
    size_t    n = 0;
    ssize_t ret;

    errno = 0;
	// Read lines one by one to count them.
    for (ret = 0; getline(&buffer, &n, file) != -1; ret++);

    free(buffer); // Free the buffer allocated by getline
    if (errno || fseek(file, 0, SEEK_SET))
        return -1;
    return ret;
}

// Function to read city coordinates from a file into an array.
// array: A pointer to an array of float[2] where coordinates will be stored.
// file: A pointer to the FILE stream to read from.
// Returns 0 on success, -1 on error.
int        retrieve_file(float (*array)[2], FILE *file)
{
    int tmp;
	// Loop through the file, reading two floats per line.
    for (size_t i = 0; (tmp = fscanf(file, "%f, %f\n", array[i] + 0, array[i] + 1)) != EOF; i++)
        if (tmp != 2) // If fscanf didn't read exactly two floats, it's an error.
        {
            errno = EINVAL;
            return -1;
        }
	// Check for any other file reading errors (e.g., I/O error).
    if (ferror(file))
        return -1;
    return 0;
}

int        main(int ac, char **av)
{
    char *filename = "stdin"; // Default input is standard input.
    FILE *file = stdin; // Default file pointer is stdin.
	// If a filename is provided as a command-line argument, open that file.
    if (ac > 1)
    {
        filename = av[1];
        file = fopen(filename, "r");
    }
	// Check if the file was opened successfully.
    if (!file)
    {
        fprintf(stderr, "Error opening %s: %m\n", filename);
        return 1;
    }
	// Determine the number of cities (lines) in the file.
    ssize_t size = file_size(file);
    if (size == -1)
    {
        fprintf(stderr, "Error reading %s: %m\n", filename);
        fclose(file);
        return 1;
    }
	// Allocate memory for the array to store city coordinates.
    float (*array)[2] = calloc(size, sizeof (float [2]));
    if (!array)
    {
        fprintf(stderr, "Error: %m\n");
        fclose(file);
        return 1;
    }
	// Retrieve city coordinates from the file into the array.
    if (retrieve_file(array, file) == -1)
    {
        fprintf(stderr, "Error reading %s: %m\n", av[1]);
        fclose(file);
        free(array);
        return 1;
    }
	// Close the file if it was opened (i.e., not stdin).
    if (ac > 1)
        fclose(file);

    // Calculate and print the shortest path length, formatted to two decimal places.
    printf("%.2f\n", tsp(array,	 size));
    free(array);
    return (0);
}




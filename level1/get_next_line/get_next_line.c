#include <stddef.h> // size_t
#include <unistd.h> // read
#include <stdlib.h> // malloc, free

// No fcntl.h or stdio.h as they are not in the allowed functions list.

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 5
#endif

// Define a maximum number of file descriptors to handle.
// This is essential for storing leftover data per file descriptor.
#define FD_MAX 1024 // A common limit for file descriptors

// --- Helper Functions ---

// ft_strlen: Calculates the length of a null-terminated string.
// Returns 0 if the input string is NULL.
size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (!s) // Handle NULL string input gracefully
		return (0);
	while (s[i])
		i++;
	return (i);
}

// ft_strchr: Locates the first occurrence of a character in a string.
// Returns a pointer to the first occurrence of 'c' in 's', or NULL if not found.
// Handles finding the null terminator and NULL input string.
char	*ft_strchr(const char *s, int c)
{
	unsigned char	uc;

	if (!s) // Handle NULL string input
		return (NULL);
	uc = (unsigned char)c;
	while (*s)
	{
		if (*s == uc)
			return ((char *)s);
		s++;
	}
	// If 'c' is the null terminator, return a pointer to the null terminator.
	if (uc == '\0')
		return ((char *)s);
	return (NULL);
}

// ft_strjoin: Joins two strings into a new dynamically allocated string.
// It frees 's1' (the old storage content) after copying, preventing leaks.
// Returns a pointer to the new joined string, or NULL on malloc failure.
char	*ft_strjoin(char *s1, char const *s2)
{
	size_t	l1;
	size_t	l2;
	char	*dest;
	size_t	i;
	size_t	j;

	l1 = 0;
	l2 = 0;
	if (s1) // If s1 is not NULL, get its length
		l1 = ft_strlen(s1);
	if (s2) // If s2 is not NULL, get its length (s2 will be read_buffer, generally not NULL)
		l2 = ft_strlen(s2);
	dest = malloc(sizeof(char) * (l1 + l2 + 1)); // Allocate memory for combined string + null terminator
	if (!dest)
		return (free(s1), NULL); // Free s1 to prevent leak if malloc fails, then return NULL
	i = 0;
	j = 0;
	while (j < l1) // Copy s1 into dest
		dest[i++] = s1[j++];
	j = 0;
	while (j < l2) // Copy s2 into dest
		dest[i++] = s2[j++];
	dest[i] = '\0'; // Null-terminate the new string
	return (free(s1), dest); // Free the old s1, then return the new dest string
}

// ft_substr: Extracts a substring from a string.
// Returns a new dynamically allocated string containing the substring,
// or NULL on malloc failure. Returns an empty string if start is out of bounds.
char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	slen;
	size_t	i;

	if (!s) // Handle NULL input string
		return (NULL);
	slen = ft_strlen(s);
	// If start index is beyond the string length, the substring has 0 length.
	if (start >= slen)
		len = 0;
	// Adjust 'len' if the requested substring goes beyond the end of 's'.
	else if (len > slen - start)
		len = slen - start;
	sub = malloc(sizeof(char) * (len + 1)); // Allocate memory for substring + null terminator
	if (!sub)
		return (NULL); // Return NULL on malloc failure
	i = 0;
	while (i < len) // Copy characters from s to sub
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0'; // Null-terminate the substring
	return (sub);
}

// helper function that extracts one line (including the "\n" if present)
// from storage, updates storage with the leftover content,
// and returns the extracted line.
char	*extract_line_and_update_storage(char **storage_ptr) // Renamed parameter to be clearer
{
	size_t	i;
	char	*line;
	char	*leftover;

	if (!*storage_ptr) // If the storage string itself is NULL, no line to extract
		return (NULL);
	i = 0;
	// Find the end of the line (either '\n' or null terminator)
	while ((*storage_ptr)[i] && (*storage_ptr)[i] != '\n')
		i++;
	// If '\n' is found, include it in the line length
	if ((*storage_ptr)[i] == '\n')
		i++;
	
	// If no characters were extracted (e.g., storage was "\0" or empty after previous operations)
	// then it means there's no valid line to return. Free storage and return NULL.
	if (i == 0) // This implicitly handles cases where *storage_ptr is an empty string
		return (free(*storage_ptr), *storage_ptr = NULL, NULL);

	// Extract the line from the beginning of storage
	line = ft_substr(*storage_ptr, 0, i);
	if (!line) // Handle malloc failure for line extraction
		return (free(*storage_ptr), *storage_ptr = NULL, NULL);

	// If the extracted line consumed all of storage (i.e., storage[i] is '\0'),
	// then storage is now empty. Free it and set to NULL.
	if (!(*storage_ptr)[i])
		return (free(*storage_ptr), *storage_ptr = NULL, line);

	// Otherwise, there's leftover content in storage.
	// Extract the leftover part.
	leftover = ft_substr(*storage_ptr, i, ft_strlen(*storage_ptr + i));
	if (!leftover) // Handle malloc failure for leftover extraction
		return (free(*storage_ptr), *storage_ptr = NULL, free(line), NULL);

	free(*storage_ptr); // Free the old storage content
	*storage_ptr = leftover; // Update storage_ptr to point to the new leftover string
	return (line); // Return the extracted line
}

// get_next_line.c
char	*get_next_line(int fd)
{
	// CRITICAL FIX: Use an array of static pointers to manage leftover data per file descriptor.
	// Each element storage[fd] holds the leftover content for a specific fd.
	static char	*storage[FD_MAX] = {NULL}; // Initialize all elements to NULL

	char		*read_buffer;
	ssize_t		bytes_read;

	// 1. Input Validation:
	// Check for invalid file descriptor or invalid BUFFER_SIZE.
	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	
	// Allocate memory for the buffer used to read data in chunks.
	read_buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!read_buffer) // Handle malloc failure
		return (NULL);

	// 2. Read Loop: Read data into storage[fd] until a newline is found or EOF.
	// The loop continues as long as there's no newline in the current storage for 'fd'.
	while (!ft_strchr(storage[fd], '\n'))
	{
		bytes_read = read(fd, read_buffer, BUFFER_SIZE);
		
		// Handle read errors:
		if (bytes_read < 0)
		{
			free(read_buffer);
			// Free the storage[fd] content and set it to NULL to prevent leaks.
			if (storage[fd]) { 
				free(storage[fd]);
				storage[fd] = NULL;
			}
			return (NULL); // Return NULL on read error
		}
		// Handle End Of File (EOF):
		if (bytes_read == 0)
			break ; // No more data to read, exit the loop

		read_buffer[bytes_read] = '\0'; // Null-terminate the read chunk
		
		// Append the newly read data to storage[fd].
		// ft_strjoin handles freeing the old storage[fd] content.
		storage[fd] = ft_strjoin(storage[fd], read_buffer);
		if (!storage[fd]) // Handle malloc failure from ft_strjoin
		{
			free(read_buffer);
			return (NULL);
		}
	}
	free(read_buffer); // Free the read_buffer as it's no longer needed

	// 3. Extract Line and Update Storage:
	// If storage[fd] is empty or became empty after reading, return NULL.
	if (!storage[fd] || *storage[fd] == '\0')
		return (free(storage[fd]), storage[fd] = NULL, NULL); // Free storage if it's empty

	// Use the helper function to extract the line and update storage[fd] with leftover.
	return (extract_line_and_update_storage(&storage[fd]));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_filter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didguill <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 11:51:00 by didguill          #+#    #+#             */
/*   Updated: 2025/10/08 12:52:24 by didguill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE (10000 + 1)

int	ft_strncmp(char	*s1, char *s2, int n)
{
	int	i;
	
	i = 0;
	while (i < n && s1[i] == s2[i])
		i++;
	if (i == n)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2 || argv[1] == NULL || strlen(argv[1]) == 0)
		return 1;

	char	*buff = (char *)malloc(BUFFER_SIZE);
	if (buff == NULL)
	{
		fprintf(stderr, "Error: ");
		perror("");
		return 1;
	}

	char	c;
	int	bytes;
	int	i;

	i = 0;
	while ((bytes = read(STDIN_FILENO, &c, 1)) > 0)
	{
		if (i >= BUFFER_SIZE -1)
			break ;
		buff[i] = c;
		i++;
	}
	if (bytes == -1)
	{
		fprintf(stderr, "Error: ");
		perror("");
		free(buff);
		return 1;
	}
	buff[i] = '\0';

	char	*search = argv[1];
	int	search_len = strlen(search);
	int	j;
	int	z;

	j = 0;
	while (buff[j])
	{
		if (j + search_len <= i && ft_strncmp(&buff[j], search, search_len) == 1)
		{
			z = 0;
			while (z < search_len)
			{
				fprintf(stdout, "*");
				z++;
			}
			j += search_len;
		}
		else
		{
			fprintf(stdout, "%c", buff[j]);
			j++;
		}
	}
	free(buff);
	return (0);
}

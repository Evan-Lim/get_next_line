/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elim-hon <elim-hon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 08:04:53 by elim-hon          #+#    #+#             */
/*   Updated: 2026/09/22 09:56:56 by elim-hon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*free_and_return(char *s1, char *s2)
{
	free(s1);
	free(s2);
	return (NULL);
}

static char	*read_to_string(int fd, char *str)
{
	char	*buff;
	char	*temp;
	int		bytes;

	buff = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buff)
		return (free_and_return(str, NULL));
	bytes = 1;
	while (!ft_strchr(str, '\n') && bytes != 0)
	{
		bytes = read(fd, buff, BUFFER_SIZE);
		if (bytes == -1)
			return (free_and_return(str, buff));
		buff[bytes] = '\0';
		temp = ft_strjoin(str, buff);
		if (!temp)
			return (free_and_return(str, buff));
		free(str);
		str = temp;
	}
	free(buff);
	return (str);
}

static char	*extract_line(char *str)
{
	int		i;
	char	*line;

	i = 0;
	if (!str[i])
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		line = (char *)malloc(sizeof(char) * (i + 2));
	else
		line = (char *)malloc(sizeof(char) * (i + 1));
	i = 0;
	while (str[i] && str[i] != '\n')
	{
		line[i] = str[i];
		i++;
	}
	if (str[i] == '\n')
	{
		line[i] = '\n';
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*update_string(char *str)
{
	int		i;
	int		j;
	char	*rest;

	i = 0;
	while (str[i] && str[i] != '\n')
		i++;
	if (!str[i])
	{
		free(str);
		return (NULL);
	}
	i++;
	rest = (char *)malloc(sizeof(char) * (ft_strlen(str) - i + 1));
	if (!rest)
		return (NULL);
	j = 0;
	while (str[i])
		rest[j++] = str[i++];
	rest[j] = '\0';
	free(str);
	return (rest);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!str)
	{
		str = (char *)malloc(1);
		if (!str)
			return (NULL);
		str[0] = '\0';
	}
	str = read_to_string(fd, str);
	if (!str)
		return (NULL);
	line = extract_line(str);
	if (!line)
	{
		free(str);
		str = NULL;
		return (NULL);
	}
	str = update_string(str);
	return (line);
}

/*
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test.txt", O_RDONLY);
	if (fd == -1)
	{
		printf("Error opening file\n");
		return (1);
	}
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		printf("[NL]\n");
		free(line);
	}
	close(fd);
	return (0);
}
*/
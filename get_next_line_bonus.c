/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elim-hon <elim-hon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 08:04:53 by elim-hon          #+#    #+#             */
/*   Updated: 2026/09/22 10:23:14 by elim-hon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

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
	static char	*str[FD_MAX];
	char		*line;

	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	if (!str[fd])
	{
		str[fd] = (char *)malloc(1);
		if (!str[fd])
			return (NULL);
		str[fd][0] = '\0';
	}
	str[fd] = read_to_string(fd, str[fd]);
	if (!str[fd])
		return (NULL);
	line = extract_line(str[fd]);
	if (!line)
	{
		free(str[fd]);
		str[fd] = NULL;
		return (NULL);
	}
	str[fd] = update_string(str[fd]);
	return (line);
}

/*
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd1;
	int		fd2;
	char	*line1;
	char	*line2;

	fd1 = open("file1.txt", O_RDONLY);
	fd2 = open("file2.txt", O_RDONLY);
	if (fd1 == -1 || fd2 == -1)
	{
		printf("Error opening files\n");
		return (1);
	}
	while (1)
	{
		line1 = get_next_line(fd1);
		line2 = get_next_line(fd2);
		if (!line1 && !line2)
			break ;
		if (line1)
		{
			printf("FD 1: %s", line1);
			free(line1);
		}
		if (line2)
		{
			printf("FD 2: %s", line2);
			free(line2);
		}
	}
	close(fd1);
	close(fd2);
	return (0);
}
*/
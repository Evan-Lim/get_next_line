/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elim-hon <elim-hon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 08:04:53 by elim-hon          #+#    #+#             */
/*   Updated: 2026/09/23 07:56:51 by elim-hon         ###   ########.fr       */
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
	{
		free(str);
		return (NULL);
	}
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "get_next_line_bonus.h"

int	main(void)
{
	int		a[2];
	int		b[2];
	char	*la; 
	char	*lb;

	pipe(a); 
	pipe(b);
	write(a[1], "A1\nA2\nA3\n", 9);   close(a[1]);
	write(b[1], "B1\nB2\nB3\nB4\n", 12); close(b[1]);
	la = get_next_line(a[0]);
	lb = get_next_line(b[0]);
	while (la || lb)
	{
		printf("A=[%s]\nB=[%s]\n", la ? la : "(null)\n", lb ? lb : "(null)\n");
		printf("[SEP]\n");
		free(la); 
		free(lb);
		la = get_next_line(a[0]);
		lb = get_next_line(b[0]);
	}
	close(a[0]); 
	close(b[0]);
	return (0);
}
*/

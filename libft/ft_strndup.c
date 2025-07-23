/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltheveni <ltheveni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 11:41:41 by ltheveni          #+#    #+#             */
/*   Updated: 2025/01/15 14:29:14 by ltheveni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

char	*ft_strndup(const char *s1, size_t n)
{
	size_t	i;
	char	*result;

	if (!s1)
		return (NULL);
	result = (char *)malloc(sizeof(char) * (n + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < n)
	{
		result[i] = s1[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

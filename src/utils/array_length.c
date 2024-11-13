/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_length.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 17:07:59 by nholbroo          #+#    #+#             */
/*   Updated: 2024/11/13 17:08:38 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	array_length(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
		i++;
	return (i);
}

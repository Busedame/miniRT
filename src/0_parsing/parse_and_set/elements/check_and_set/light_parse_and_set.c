/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light_parse_and_set.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:15:08 by nholbroo          #+#    #+#             */
/*   Updated: 2025/01/07 18:11:21 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/*Checks if the light brightness is in between the range of 0 and 1.
Returns 0 if it is within the range.
Returns an error code if it's too small or too big, if mem allocation
failed, or if consists of anything else than number, '+', '-' or dec point..*/
static int	light_brightness(t_scene *scene, int *parsing_error)
{
	char	*sub;

	if (!only_numbers_dec_pt_and_newline(scene->pars.elem_data[2]))
		return (set_error_and_return(NULL, &parsing_error, \
		ERR_LIGHT_BRIGHTNESS));
	if (ft_strchr(scene->pars.elem_data[2], '\n'))
	{
		sub = ft_substr(scene->pars.elem_data[2], 0, \
		ft_strchr_index(scene->pars.elem_data[2], '\n'));
		if (!sub)
			return (set_error_and_return(NULL, &parsing_error, ERR_MEM_ALLOC));
		scene->light.ratio = ft_atod(sub);
		free(sub);
	}
	else
		scene->light.ratio = ft_atod(scene->pars.elem_data[2]);
	if (scene->light.ratio < 0 || scene->light.ratio > 1)
		return (set_error_and_return(NULL, &parsing_error, \
		ERR_LIGHT_BRIGHTNESS));
	return (0);
}

/*Checks if the current value is greater than or equal to 0 or
less than 255.
Returns 0 and sets the value if it is within the range.
Returns 1 if it's too small or too big.*/
static int	set_rgb_value_light(char *input_value, t_scene *scene, int rgb)
{
	int	value;

	value = ft_atoi(input_value);
	if (value < 0 || value > 255)
		return (1);
	if (rgb == 0)
		scene->light.color.r = value;
	else if (rgb == 1)
		scene->light.color.g = value;
	else if (rgb == 2)
		scene->light.color.b = value;
	return (0);
}

/*Checks if the rgb values of the given object are valid.
1. Checks if there are 3 values.
2. Checks if they all consist of only numbers and/or a newline (as the last
number will be followed by a newline).
3. Checks if they are all in range of 0 to 255.
If an error occurs, a correct error code is set.*/
static int	light_color(t_scene *scene, char **rgb)
{
	if (array_length(rgb) != 3 || rgb[2][0] == '\n')
		return (ERR_LIGHT_COLOR_FIELDS);
	if (!only_numbers_and_newline(rgb[0]) || !only_numbers_and_newline(rgb[1])
		|| !only_numbers_and_newline(rgb[2]))
		return (ERR_LIGHT_COLOR_VALUES);
	if (set_rgb_value_light(rgb[0], scene, 0) != 0)
		return (ERR_LIGHT_COLOR_VALUES);
	if (set_rgb_value_light(rgb[1], scene, 1) != 0)
		return (ERR_LIGHT_COLOR_VALUES);
	if (set_rgb_value_light(rgb[2], scene, 2) != 0)
		return (ERR_LIGHT_COLOR_VALUES);
	return (0);
}

/*Parses and sets a light object.
1. Checks if there's a correct amount of fields (3 or 4).
2. Checks if the coordinates are valid.
3. Checks if the light brightness is valid (between 0 and 1).
4. Checks if the color values are valid (only if there are 4 fields).
Sets all values and returns 0 upon success, and an error code upon error.*/
int	parse_and_set_light(t_scene *scene)
{
	int		arr_len;
	char	**rgb;

	arr_len = array_length(scene->pars.elem_data);
	if (!correct_amt_of_fields(scene->pars.elem_data, 3)
		&& !correct_amt_of_fields(scene->pars.elem_data, 4))
		return (ERR_LIGHT_FIELDS);
	if (check_coordinates(scene->pars.elem_data[1], \
	&scene->pars.error_code, ERR_LIGHT_COOR_FIELDS) != 0)
		return (scene->pars.error_code);
	if (light_brightness(scene, &scene->pars.error_code) != 0)
		return (scene->pars.error_code);
	if (arr_len == 4 && scene->pars.elem_data[3][0] != '\n')
	{
		rgb = ft_split(scene->pars.elem_data[3], ',');
		if (!rgb)
			return (ERR_MEM_ALLOC);
		scene->pars.error_code = light_color(scene, rgb);
		ft_freearray(rgb);
		if (scene->pars.error_code != 0)
			return (scene->pars.error_code);
	}
	return (0);
}

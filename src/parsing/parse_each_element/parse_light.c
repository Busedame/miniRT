/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:15:08 by nholbroo          #+#    #+#             */
/*   Updated: 2024/11/19 18:11:30 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	light_coordinates(t_scene *scene)
{
	char	**coords;
	int		i;

	i = 0;
	coords = ft_split(scene->pars.elem_data[1], ',');
	if (!coords)
		return (ERR_MEM_ALLOC);
	if (array_length(coords) != 3)
		return (ERR_LIGHT_COOR_FIELDS);
	if (!only_numbers_single_signs_and_dec_pt(coords[0])
		|| !only_numbers_single_signs_and_dec_pt(coords[1])
		|| !only_numbers_single_signs_and_dec_pt(coords[2]))
	{
		ft_freearray(coords);
		return (ERR_LIGHT_COOR_VALUES);
	}
	scene->light.x = ft_atod(coords[0]);
	scene->light.y = ft_atod(coords[1]);
	scene->light.z = ft_atod(coords[2]);
	ft_freearray(coords);
	return (0);
}

static int	light_brightness(t_scene *scene)
{
	char	*sub;

	if (!only_numbers_dec_pt_and_newline(scene->pars.elem_data[2]))
		return (ERR_LIGHT_BRIGHTNESS);
	if (ft_strchr(scene->pars.elem_data[2], '\n'))
	{
		sub = ft_substr(scene->pars.elem_data[2], 0, \
		ft_strchr_index(scene->pars.elem_data[2], '\n'));
		if (!sub)
			return (ERR_MEM_ALLOC);
		scene->light.bright = ft_atod(sub);
		free(sub);
	}
	else
		scene->light.bright = ft_atod(scene->pars.elem_data[2]);
	if (scene->light.bright < 0 || scene->light.bright > 1)
		return (ERR_LIGHT_BRIGHTNESS);
	return (0);
}

static int	set_rgb_value_light(char *input_value, t_scene *scene, int rgb)
{
	int	value;

	value = ft_atoi(input_value);
	if (value < 0 || value > 255)
		return (1);
	if (rgb == 0)
		scene->light.color_r = value;
	else if (rgb == 1)
		scene->light.color_g = value;
	else if (rgb == 2)
		scene->light.color_b = value;
	return (0);
}

static int	light_color(t_scene *scene, char **rgb)
{
	int		value;

	value = 0;
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

int	parse_light(t_scene *scene)
{
	int		arr_len;
	char	**rgb;

	rgb = NULL;
	arr_len = array_length(scene->pars.elem_data);
	if (!correct_amt_of_fields(scene->pars.elem_data, 3)
		&& !correct_amt_of_fields(scene->pars.elem_data, 4))
		return (ERR_LIGHT_FIELDS);
	scene->pars.error_code = light_coordinates(scene);
	if (scene->pars.error_code != 0)
		return (scene->pars.error_code);
	scene->pars.error_code = light_brightness(scene);
	if (scene->pars.error_code != 0)
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
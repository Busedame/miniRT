/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:15:08 by nholbroo          #+#    #+#             */
/*   Updated: 2024/11/18 18:19:13 by nholbroo         ###   ########.fr       */
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
	scene->light.x = ft_atof(coords[0]);
	scene->light.y = ft_atof(coords[1]);
	scene->light.z = ft_atof(coords[2]);
	ft_freearray(coords);
	return (0);
}

static int	light_brightness(t_scene *scene)
{
	if (!only_numbers_dec_pt_and_newline(scene->pars.elem_data[2]))
		return (ERR_LIGHT_BRIGHTNESS);
	scene->light.bright = ft_atof(scene->pars.elem_data[2]);
	if (scene->light.bright < 0 || scene->light.bright > 1)
		return (ERR_LIGHT_BRIGHTNESS);
	return (0);
}

static int	light_color(t_scene *scene, char **rgb)
{
	int		value;

	value = 0;
	if (array_length(rgb) != 3)
		return (ERR_LIGHT_COLOR_FIELDS);
	if (!only_numbers_and_newline(rgb[0]) || !only_numbers_and_newline(rgb[1])
		|| !only_numbers_and_newline(rgb[2]))
		return (ERR_LIGHT_COLOR_VALUES);
	value = ft_atoi(rgb[0]);
	if (value < 0 || value > 255)
		return (ERR_LIGHT_COLOR_VALUES);
	scene->light.color_r = value;
	value = ft_atoi(rgb[1]);
	if (value < 0 || value > 255)
		return (ERR_LIGHT_COLOR_VALUES);
	scene->light.color_g = value;
	value = ft_atoi(rgb[2]);
	if (value < 0 || value > 255)
		return (ERR_LIGHT_COLOR_VALUES);
	scene->light.color_b = value;
	return (0);
}

int	parse_light(t_scene *scene)
{
	int		arr_len;
	char	**rgb;

	rgb = NULL;
	arr_len = array_length(scene->pars.elem_data);
	if (arr_len != 3 && arr_len != 4)
		return (ERR_LIGHT_FIELDS);
	scene->pars.error_code = light_coordinates(scene);
	if (scene->pars.error_code != 0)
		return (scene->pars.error_code);
	scene->pars.error_code = light_brightness(scene);
	if (scene->pars.error_code != 0)
		return (scene->pars.error_code);
	if (arr_len == 4)
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

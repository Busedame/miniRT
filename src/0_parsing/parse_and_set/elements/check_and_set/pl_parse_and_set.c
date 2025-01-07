/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pl_parse_and_set.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 19:50:27 by nholbroo          #+#    #+#             */
/*   Updated: 2025/01/07 18:09:33 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/* Stores special attributes for a plane in the object data. */
static void	set_special_attributes_plane(t_scene *scene, t_obj_data *obj_data)
{
	obj_data->pl.ixd.difference = vec3_sub(obj_data->pl.point_in_plane, \
		scene->cam.pos);
	obj_data->pl.hex_color = color_to_hex(obj_data->pl.color);
	obj_data->pl.ixd.dot_diff_normal = vec3_dot(obj_data->pl.ixd.difference, \
		obj_data->pl.normal);
}

/*Stores all the data of the current plane in the linked list of objects.*/
int	set_plane(t_scene *scene)
{
	t_obj_data	*obj_data;
	char		**rgb;

	obj_data = malloc(sizeof(t_obj_data));
	if (!obj_data)
		return (ERR_MEM_ALLOC);
	obj_data->pl.object_type = PLANE;
	if (set_coordinates(scene->pars.elem_data[1], \
	&obj_data->pl.point_in_plane.x, &obj_data->pl.point_in_plane.y, \
	&obj_data->pl.point_in_plane.z) != 0)
		return (ERR_MEM_ALLOC);
	if (set_orientation_vector(scene->pars.elem_data[2], \
		&obj_data->pl.normal.x, &obj_data->pl.normal.y, \
		&obj_data->pl.normal.z))
		return (ERR_MEM_ALLOC);
	rgb = ft_split(scene->pars.elem_data[3], ',');
	if (!rgb)
		return (ERR_MEM_ALLOC);
	set_color(rgb, &obj_data->pl.color.r, &obj_data->pl.color.g, \
	&obj_data->pl.color.b);
	set_special_attributes_plane(scene, obj_data);
	if (add_to_object_list(&scene, &obj_data) != 0)
		return (ERR_MEM_ALLOC);
	return (0);
}

/*Parses a plane object.
1. Checks if there's a correct amount of fields (4).
2. Checks if the coordinates are valid.
3. Checks if the orientation vector is valid.
4. Checks if the color values are valid.
Also keeps track of the count, ultimately storing the total count (how
many occurences of the object), to be able to allocate memory correctly
later on.
Returns 0 upon success, and an error code upon error.*/
int	parse_plane(t_scene *scene)
{
	char	**rgb;

	scene->tot_pl++;
	if (!correct_amt_of_fields(scene->pars.elem_data, 4))
		return (ERR_PL_FIELDS);
	if (check_coordinates(scene->pars.elem_data[1], \
	&scene->pars.error_code, ERR_PL_COOR_FIELDS) != 0)
		return (scene->pars.error_code);
	if (check_orientation_vector(scene->pars.elem_data[2], \
	&scene->pars.error_code, ERR_PL_VECTOR_FIELDS) != 0)
		return (scene->pars.error_code);
	rgb = ft_split(scene->pars.elem_data[3], ',');
	if (!rgb)
		return (ERR_MEM_ALLOC);
	if (check_color(rgb, &scene->pars.error_code, ERR_PL_COLOR_FIELDS) != 0)
	{
		ft_freearray(rgb);
		return (scene->pars.error_code);
	}
	ft_freearray(rgb);
	return (set_plane(scene));
}

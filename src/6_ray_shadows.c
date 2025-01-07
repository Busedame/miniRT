/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   6_ray_shadows.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 11:37:01 by nholbroo          #+#    #+#             */
/*   Updated: 2025/01/07 18:48:33 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/**
Utility struct to store the closest intersection distance and its color.
- double `t_closest`:	The closest intersection distance for a ray.
- int `ixn_color`:		The color of the closest intersection.
*/
typedef struct s_intersection_result
{
	double	t_closest;
	int		ixn_color;
}	t_ixr;

t_vec3 calculate_cylinder_normal(t_vec3 intersection_point, t_obj_data *obj_data)
{
	t_cylinder *cy = &obj_data->cy;
	t_vec3 point_on_axis;
	t_vec3 normal;
	double projection_length;

	projection_length = vec3_dot(vec3_sub(intersection_point, cy->center), cy->orientation);

	if (projection_length > 0 && projection_length < cy->height)
	{
		point_on_axis = vec3_add(cy->center, vec3_scale(cy->orientation, projection_length));
		normal = vec3_sub(intersection_point, point_on_axis);
		return vec3_norm(normal);
	}

	if (projection_length >= cy->height)
	{
		return cy->cap_top_normal;
	}

    return cy->cap_bottom_normal;
}

t_vec3	get_normal_at_point(t_vec3 point, t_obj_data *obj_data)
{
	if (obj_data->pl.object_type == PLANE)
		return (obj_data->pl.normal);
	else if (obj_data->sp.object_type == SPHERE)
		return (vec3_norm(vec3_sub(point, obj_data->sp.center)));
	else if (obj_data->cy.object_type == CYLINDER)
		return (calculate_cylinder_normal(point, obj_data));
	return ((t_vec3){0, 0, 0});
}

t_vec3	calculate_ray_direction_to_light(t_rt *rt, t_vec3 ray_dir, double t, 
	t_obj_data *obj_data)
{
	t_vec3	intersection_point;
	t_vec3	light_dir;
	t_vec3	normal;
	t_vec3	offset_origin;

	intersection_point = vec3_add(rt->scene.cam.pos, vec3_scale(ray_dir, t));
	normal = get_normal_at_point(intersection_point, obj_data);
	offset_origin = vec3_add(intersection_point, vec3_scale(normal, 1e-4));
	light_dir = vec3_sub(rt->scene.light.position, offset_origin);
	return (vec3_norm(light_dir));
}

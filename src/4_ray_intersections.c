/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_ray_intersections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nholbroo <nholbroo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:59:09 by aschenk           #+#    #+#             */
/*   Updated: 2025/01/07 18:52:29 by nholbroo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
Handles ray-object intersection calculations for planes, spheres, and cylinders
in the parsed object list. Determines the closest intersection distance and its
corresponding color.
*/

#include "main.h"

// IN FILE:

int	find_closest_intersection(t_vec3 ray_dir, t_rt *rt);

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

/**
Checks for intersection of a ray with a plane object.

If an intersection occurs and is closer than the current closest distance,
updates the closest intersection distance and the intersection color in the
provided struct.

 @param ray_dir		The direction vector of the ray.
 @param obj_data	The object data containing the plane information.
 @param ixr			Pointer to the intersection struct to update.
*/
static void	check_plane_intersection(t_rt *rt, t_vec3 ray_dir, 
		t_obj_data *obj_data, t_ixr *ixr)
{
	double	t;

	if (ray_intersect_plane(ray_dir, &obj_data->pl, &t) && t < ixr->t_closest)
	{
		ixr->t_closest = t;
		ixr->ixn_color = obj_data->pl.hex_color;
	}
}

/**
Checks for intersection of a ray with a sphere object.

If an intersection occurs and is closer than the current closest distance,
updates the closest intersection distance and the intersection color in the
provided struct.

 @param ray_dir		The direction vector of the ray.
 @param obj_data	The object data containing the sphere information.
 @param ixr			Pointer to the intersection struct to update.
*/
static void	check_sphere_intersection(t_rt *rt, t_vec3 ray_dir, 
		t_obj_data *obj_data, t_ixr *ixr)
{
	double	t;

	if (ray_intersect_sphere(ray_dir, &obj_data->sp, &t) && t < ixr->t_closest)
	{
		ixr->t_closest = t;
		ixr->ixn_color = obj_data->sp.hex_color;
	}
}

/**
Checks for intersection of a ray with a cylinder object, including its caps.

If an intersection occurs and is closer than the current closest distance,
updates the closest intersection distance and the intersection color in the
provided struct.

 @param ray_origin	The origin of the ray.
 @param ray_dir		The direction vector of the ray.
 @param obj_data	The object data containing the cylinder information.
 @param ixr			Pointer to the intersection struct to update.

 @note
The function avoids unnecessary checks by skipping the bottom cap test if the
top cap is already marked as hit, and vice versa.
This ensures that only one cap can be marked as hit per cylinder intersection
check, as only one cap can be visible at a time in the rendering.
*/
static void	check_cyl_intersection(t_rt *rt, t_vec3 ray_dir,
		t_obj_data *obj_data, t_ixr *ixr)
{
	double		t;

	if (ray_intersect_cylinder(rt->scene.cam.pos, ray_dir, &obj_data->cy, &t)
		&& t < ixr->t_closest)
	{
		ixr->t_closest = t;
		ixr->ixn_color = obj_data->cy.hex_color;
	}
	if (obj_data->cy.ixd.cap_hit != 2 && 
		ray_intersect_cap_top(rt->scene.cam.pos, ray_dir, &obj_data->cy, 
			&t) && t < ixr->t_closest)
	{
		ixr->t_closest = t;
		ixr->ixn_color = obj_data->cy.hex_color;
		obj_data->cy.ixd.cap_hit = 1;
	}
	if (obj_data->cy.ixd.cap_hit != 1 
		&& ray_intersect_cap_bottom(rt->scene.cam.pos, ray_dir, 
			&obj_data->cy, &t) && t < ixr->t_closest)
	{
		ixr->t_closest = t;
		ixr->ixn_color = obj_data->cy.hex_color;
		obj_data->cy.ixd.cap_hit = 2;
	}
}

static int	ray_is_in_shadow(t_rt *rt, t_vec3 light_dir)
{
	t_list		*current_obj;
	t_obj_data	*obj_data;
	t_ixr		ixr;

	current_obj = rt->scene.objs;
	ixr.t_closest = INFINITY;
	ixr.ixn_color = -1;
	while (current_obj != NULL)
	{
		obj_data = (t_obj_data *)current_obj->content;
		if (obj_data->pl.object_type == PLANE)
			check_plane_intersection(rt, light_dir, obj_data, &ixr);
		else if (obj_data->sp.object_type == SPHERE)
			check_sphere_intersection(rt, light_dir, obj_data, &ixr);
		else if (obj_data->cy.object_type == CYLINDER)
			check_cyl_intersection(rt, light_dir, obj_data, &ixr);
		current_obj = current_obj->next;
	}
	if (ixr.ixn_color == -1)
		return (0);
	return (1);
}

static void	shadow(t_rt *rt, t_vec3 ray_dir, t_obj_data *obj_data, t_ixr *ixr)
{
	t_vec3	light_dir;

	light_dir = calculate_ray_direction_to_light(rt, ray_dir, ixr->t_closest, 
			obj_data);
	if (ray_is_in_shadow(rt, light_dir))
		ixr->ixn_color = 0;
}

/**
Computes the color of the closest intersection with objects or uses ambient light
if no intersections occur.

 @param ray_dir 	The direction vector of the current ray.
 @param rt 			Pointer to the main program state structure.
 @param t_closest 	Pointer to the closest intersection distance (updated).

 @return 			The computed color for the closest intersection;
 					or ambient light when no intersections occur.
*/
int	find_closest_intersection(t_vec3 ray_dir, t_rt *rt)
{
	t_list		*current_obj;
	t_obj_data	*obj_data;
	t_ixr		ixr;

	current_obj = rt->scene.objs;
	ixr.t_closest = INFINITY;
	ixr.ixn_color = -1;
	while (current_obj != NULL)
	{
		obj_data = (t_obj_data *)current_obj->content;
		if (obj_data->pl.object_type == PLANE)
			check_plane_intersection(rt, ray_dir, obj_data, &ixr);
		else if (obj_data->sp.object_type == SPHERE)
			check_sphere_intersection(rt, ray_dir, obj_data, &ixr);
		else if (obj_data->cy.object_type == CYLINDER)
			check_cyl_intersection(rt, ray_dir, obj_data, &ixr);
		current_obj = current_obj->next;
	}
	if (ixr.ixn_color == -1)
		ixr.ixn_color = BG_COLOR;
	else
		shadow(rt, ray_dir, obj_data, &ixr);
	return (ixr.ixn_color);
}

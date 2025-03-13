/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_find_intersection.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/09 11:59:09 by aschenk           #+#    #+#             */
/*   Updated: 2025/03/03 17:03:31 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Handles camera ray -> object intersection for planes, spheres, and cylinders
in the parsed object list. Determines the intersection data for the closest
object hit by the ray.
*/

#include "main.h"

// IN FILE:

void	find_ix(t_vec3 ray_ori, t_vec3 ray_dir, t_rt *rt, t_ix *ix);

/**
Checks for intersection of a ray with a plane object.

Updates the intersection data in the provided `ixr` struct if an intersection
occurs that is closer than the current closest distance.
 @param ray_ori 	The origin of the ray.
 @param ray_dir		The normalized direction vector of the ray.
 @param obj			Pointer to the object data.
 @param ix			Pointer to the 'intersection data' struct to update.
*/
static void	plane_ix(t_vec3 ray_ori, t_vec3 ray_dir, t_obj *obj, t_ix *ix)
{
	double	t;

	if (ray_hit_plane(ray_ori, ray_dir, obj, &t) && t < ix->t_hit)
	{
		ix->t_hit = t;
		ix->hit_obj = obj;
	}
}

/**
Checks for intersection of a ray with a sphere object.

Updates the intersection data in the provided `ixr` struct if an intersection
occurs that is closer than the current closest distance.
 @param ray_ori 	The origin of the ray.
 @param ray_dir		The normalized direction vector of the ray.
 @param obj			Pointer to the object data.
 @param ix			Pointer to the 'intersection data' struct to update.
*/
static void	sphere_ix(t_vec3 ray_ori, t_vec3 ray_dir, t_obj *obj, t_ix *ix)
{
	double	t;

	if (ray_hit_sphere(ray_ori, ray_dir, obj, &t) && t < ix->t_hit)
	{
		ix->t_hit = t;
		ix->hit_obj = obj;
	}
}

/**
Checks for intersection of a ray with a cylinder object, including its caps.

Updates the intersection data in the provided `ixr` struct if an intersection
occurs that is closer than the current closest distance.
 @param ray_ori 	The origin of the ray.
 @param ray_dir		The normalized direction vector of the ray.
 @param obj			Pointer to the object data.
 @param ix			Pointer to the 'intersection data' struct to update.

 @note
The function avoids unnecessary checks by skipping the bottom cap test if the
top cap is already marked as hit, and vice versa.
This ensures that only one cap can be marked as hit per cylinder intersection
check, as only one cap can be visible at a time in the rendering.
*/
static void	cyl_ix(t_vec3 ray_origin, t_vec3 ray_dir, t_obj *obj, t_ix *ix)
{
	double		t;

	if (ray_hit_cyl(ray_origin, ray_dir, obj, &t) && t < ix->t_hit)
	{
		ix->t_hit = t;
		ix->hit_obj = obj;
		ix->cap_hit = NO_HIT;
	}
	if (ray_hit_cap_top(ray_origin, ray_dir, obj, &t) && t < ix->t_hit)
	{
		ix->t_hit = t;
		ix->hit_obj = obj;
		ix->cap_hit = TOP_CAP;
	}
	if (ray_hit_cap_bottom(ray_origin, ray_dir, obj, &t) && t < ix->t_hit)
	{
		ix->t_hit = t;
		ix->hit_obj = obj;
		ix->cap_hit = BOTTOM_CAP;
	}
}

/**
Updates the intersection struct with the closest intersection data found in the
scene (object and distance) for a given camera ray.
 @param ray_ori 	The origin of the ray.
 @param ray_dir		The normalized direction vector of the ray.
 @param obj			Pointer to the object data.
 @param ix			Pointer to the 'intersection data' struct to update.
*/
void	find_ix(t_vec3 ray_ori, t_vec3 ray_dir, t_rt *rt, t_ix *ix)
{
	t_list	*current_obj;
	t_obj	*obj;

	current_obj = rt->scene.objs;
	ix->hit_obj = NULL;
	ix->t_hit = INFINITY;
	while (current_obj)
	{
		obj = (t_obj *)current_obj->content;
		if (obj->object_type == PLANE)
			plane_ix(ray_ori, ray_dir, obj, ix);
		else if (obj->object_type == SPHERE)
			sphere_ix(ray_ori, ray_dir, obj, ix);
		else if (obj->object_type == CYLINDER)
			cyl_ix(ray_ori, ray_dir, obj, ix);
		current_obj = current_obj->next;
	}
	if (ix->hit_obj != NULL)
		ix->hit_point = vec3_add(ray_ori, vec3_mult(ray_dir, ix->t_hit));
}

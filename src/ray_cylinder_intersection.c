/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_cylinder_intersection.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:47:07 by aschenk           #+#    #+#             */
/*   Updated: 2024/12/05 19:25:22 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
This file contains functions to calculate the intersection of a ray with a
cylinder object in 3D space.

See the README for more details on the mathematically derived ray-object
intersection algorithms:
https://github.com/Busedame/miniRT/blob/main/README.md#ray-object-intersection
*/

#include "main.h"

// IN FILE:

int		ray_intersect_cylinder(t_vec3 ray_origin, t_vec3 ray_dir,
			t_cylinder *cylinder, double *t);

/**
Function to calculate the coefficients of the quadratic equation and other
variables for the intersection of a ray with a cylinder.

 @param cyl 		Pointer to the cylinder structure.
 @param ray_dir 	The normalized direction vector of the ray.
 @param oc 			The vector from the ray origin to the cylinder center.

 @return			None. The function modifies the cylinder's `ixd`
 					(intersection data) structure to store the calculated
					coefficients and the discriminante.
*/
static void	compute_cylinder_intersection_vars(t_cylinder *cyl, t_vec3 ray_dir)
{
	double	axis_dot_ray;

	axis_dot_ray = vec3_dot(ray_dir, cyl->orientation);
	cyl->ixd.a = vec3_dot(ray_dir, ray_dir)
		- (axis_dot_ray * axis_dot_ray);
	cyl->ixd.b = 2 * (vec3_dot(cyl->ixd.oc, ray_dir)
			- (cyl->ixd.axis_dot_oc * axis_dot_ray));
	cyl->ixd.discriminant = calculate_discriminant(cyl->ixd.a, cyl->ixd.b,
			cyl->ixd.c);
}

/**
Function to check whether a given intersection point on an infinite cylinder
lies within the cylinder's finite height bounds.

 @param ray_origin 		The origin of the ray in 3D space.
 @param ray_dir 		The normalized direction vector of the ray.
 @param t 				The distance along the ray to the intersection point.
 @param cylinder 		Pointer to the cylinder structure.

 @return 				`1` if the intersection point lies within the cylinder's
						height bounds;
						`0` otherwise.
*/
static int	check_cylinder_height(t_vec3 ray_origin, t_vec3 ray_dir,
		t_cylinder *cylinder, double t)
{
	t_vec3	intersection_point;
	t_vec3	center_to_point;
	double	projection_length;
	double	half_height;

	intersection_point = vec3_add(ray_origin, vec3_mult(ray_dir, t));
	center_to_point = vec3_sub(intersection_point, cylinder->center);
	projection_length = vec3_dot(center_to_point, cylinder->orientation);
	half_height = cylinder->height / 2.0;
	if (projection_length >= -half_height && projection_length <= half_height)
		return (1);
	return (0);
}

/**
Function to check intersection with cylinder's top end cap.

 @param ray_origin 		The origin of the ray.
 @param ray_dir 		The normalized direction vector of the ray.
 @param cylinder 		Pointer to the cylinder structure.
 @param t 				Pointer to store the intersection distance if valid.

 @return 				`1` if the ray intersects the cap within its radius;
						`0` otherwise.
*/
int	ray_intersect_cap_top(t_vec3 ray_origin, t_vec3 ray_dir, t_cylinder *cyl,
		double *t)
{
	double	denom;
	double	t_cap;
	t_vec3	intersection_point;
	t_vec3	difference;

	denom = vec3_dot(ray_dir, cyl->cap_top_normal);
	if (fabs(denom) < 1e-6)
		return (0);
	t_cap = vec3_dot(cyl->ixd.ori_to_cap_top, cyl->cap_top_normal) / denom;
	if (t_cap < 0.0)
		return (0);
	intersection_point = vec3_add(ray_origin, vec3_mult(ray_dir, t_cap));
	difference = vec3_sub(intersection_point, cyl->cap_top_center);
	if (vec3_dot(difference, difference) <= cyl->radius_sqrd)
	{
		*t = t_cap;
		return (1);
	}
	return (0);
}

/**
Function to check intersection with cylinder's bottom end cap.

 @param ray_origin 		The origin of the ray.
 @param ray_dir 		The normalized direction vector of the ray.
 @param cylinder 		Pointer to the cylinder structure.
 @param t 				Pointer to store the intersection distance if valid.

 @return 				`1` if the ray intersects the cap within its radius;
						`0` otherwise.
*/
int	ray_intersect_cap_bottom(t_vec3 ray_origin, t_vec3 ray_dir, t_cylinder *cyl,
		double *t)
{
	double	denom;
	double	t_cap;
	t_vec3	intersection_point;
	t_vec3	difference;

	denom = vec3_dot(ray_dir, cyl->cap_bottom_normal);
	if (fabs(denom) < 1e-6)
		return (0);
	t_cap = vec3_dot(cyl->ixd.ori_to_cap_bottom, cyl->cap_bottom_normal)
		/ denom;
	if (t_cap < 0.0)
		return (0);
	intersection_point = vec3_add(ray_origin, vec3_mult(ray_dir, t_cap));
	difference = vec3_sub(intersection_point, cyl->cap_bottom_center);
	if (vec3_dot(difference, difference) <= cyl->radius_sqrd)
	{
		*t = t_cap;
		return (1);
	}
	return (0);
}

/**
Function to determine whether a ray intersects with a finite cylinder
and, if so, calculate the distance to the closest intersection point.

 @param ray_origin 		The origin of the ray in 3D space.
 @param ray_dir 		The normalized direction vector of the ray.
 @param cylinder 		Pointer to the cylinder structure.
 @param t 				Pointer to store the distance to the first intersection
						point (if found); could be the entry or exit point (if
						the ray starts inside the object ).

 @return 				`1` if the ray intersects with the cylinder within its
 						finite height bounds;
						`0` otherwise.
*/
int	ray_intersect_cylinder(t_vec3 ray_origin, t_vec3 ray_dir,
		t_cylinder *cylinder, double *t)
{
	(void)ray_origin;
	compute_cylinder_intersection_vars(cylinder, ray_dir);
	if (cylinder->ixd.discriminant < 0)
		return (0);
	*t = calculate_entry_distance(cylinder->ixd.a, cylinder->ixd.b,
			cylinder->ixd.discriminant);
	if (*t >= 0.0 && check_cylinder_height(ray_origin, ray_dir, cylinder, *t))
		return (1);
	*t = calculate_exit_distance(cylinder->ixd.a, cylinder->ixd.b,
			cylinder->ixd.discriminant);
	if (*t >= 0.0 && check_cylinder_height(ray_origin, ray_dir, cylinder, *t))
		return (1);
	return (0);
}

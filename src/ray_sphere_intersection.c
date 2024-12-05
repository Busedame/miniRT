/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_sphere_intersection.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:47:07 by aschenk           #+#    #+#             */
/*   Updated: 2024/12/05 06:52:46 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
This file contains functions to calculate the intersection of a ray with a
sphere object in 3D space.

See the README for more details on the mathematically derived ray-object
intersection algorithms used in the function below:
https://github.com/Busedame/miniRT/blob/main/README.md#ray-object-intersection
*/

#include "main.h"

// IN FILE:

int		ray_intersect_sphere(t_vec3 ray_dir, t_sphere *sphere, double *t);

/**
Function to find the intersection of a ray with a sphere.

 @param ray_origin 	The starting point of the ray (3D vector).
 @param ray_dir 	The direction vector of the ray (assumed to be normalized).
 @param sphere 		Pointer to the sphere structure.
 @param t 			Pointer to store the distance to the first intersection
					point (if found); could be the entry or exit point (if the
					ray starts inside the object).

 @return 			`1` if an intersection is found (and t is set to the
					intersection distance);
					`0` if there is no intersection.

 @note 				`a = (ray_dir . ray_dir)` is 1.0 if the ray direction
					vector is normalized.
*/
int	ray_intersect_sphere(t_vec3 ray_dir, t_sphere *sphere, double *t)
{
	sphere->ixd.b = 2.0 * vec3_dot(sphere->ixd.oc, ray_dir);
	sphere->ixd.discriminant = calculate_discriminant(1.0, sphere->ixd.b,
			sphere->ixd.c);
	if (sphere->ixd.discriminant < 0.0)
		return (0);
	*t = calculate_entry_distance(1.0, sphere->ixd.b, sphere->ixd.discriminant);
	if (*t >= 0.0)
		return (1);
	*t = calculate_exit_distance(1.0, sphere->ixd.b, sphere->ixd.discriminant);
	if (*t >= 0.0)
		return (1);
	return (0);
}

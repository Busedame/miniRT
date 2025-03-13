/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_ray_hit_plane.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:47:07 by aschenk           #+#    #+#             */
/*   Updated: 2025/03/03 15:53:00 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
This file contains functions to calculate the intersection of a ray with a
plane object in 3D space.

See the README for more details on the mathematically derived ray-object
intersection algorithms used in the function below:
https://github.com/Busedame/miniRT/blob/main/README.md#ray-object-intersection
*/

#include "main.h"

// IN FILE:

int	ray_hit_plane(t_vec3 ray_ori, t_vec3 ray_dir, t_obj *obj, double *t);

/**
Function to find the intersection of a ray with a plane.
 @param ray_ori 	The origin of the ray.
 @param ray_dir		The normalized direction vector of the ray.
 @param obj			Pointer to the object data.
 @param t 			A pointer to store the distance to the intersection point.

 @return 			`1` if the ray intersects the plane within the FOV
					(`t` is set to the intersection distance);
					`0` if there is no intersection within the FOV (ray is
					parallel to the plane or intersection behind the camera).

 @note
Due to floating-point precision limitations, directly comparing a dot product
to zero can be unreliable. A small threshold (EPSILON -> 1e-6) is used to
determine if the ray is parallel to the plane. Values below this threshold are
considered too close to zero, indicating parallelism or preventing division by
very small numbers, which could lead to inaccuracies.
*/
int	ray_hit_plane(t_vec3 ray_ori, t_vec3 ray_dir, t_obj *obj, double *t)
{
	double	denom;
	t_vec3	difference;
	double	dot_diff_normal;

	denom = vec3_dot(ray_dir, obj->x.pl.normal);
	if (fabs(denom) > EPSILON)
	{
		difference = vec3_sub(obj->x.pl.point_in_plane, ray_ori);
		dot_diff_normal = vec3_dot(difference, obj->x.pl.normal);
		*t = dot_diff_normal / denom;
		if (*t > 0.0)
			return (1);
	}
	return (0);
}

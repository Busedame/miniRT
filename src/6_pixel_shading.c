/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   6_pixel_shading.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 09:10:11 by aschenk           #+#    #+#             */
/*   Updated: 2025/03/03 15:06:25 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Functions for computing the shading of a pixel in a 3D scene.
*/

#ifdef BONUS
# include "main_bonus.h"
#else
# include "main.h"
#endif

// IN_FILE:

t_shade	get_shading(t_rt *rt, t_ix *ix);

/**
Calculates the diffuse reflection coefficient at an intersection point using
Lambert's Cosine Law.
The diffuse coefficient how much light a surface receives based on its angle
to the light source.
 @param rt 		Pointer to the main structure.
 @param ix 		Pointer to the intersection data.

 @return		The diffuse lighting coefficient value [0.0, 1.0].
*/
static double	get_diffuse_coefficient(t_rt *rt, t_ix *ix)
{
	double	diffuse;
	double	dot;

	dot = vec3_dot(ix->normal, ix->light_dir);
	if (dot < 0)
		dot = 0;
	diffuse = dot * rt->scene.light.ratio * K_DIFFUSE;
	return (diffuse);
}

/**
Computes the reflection vector based on the incoming vector and the
surface normal.
 @param vec_in 	The incoming vector.
 @param normal 	The surface normal.

 @return		The normalized reflected vector.
*/
static t_vec3	get_reflection(t_vec3 vec_in, t_vec3 normal)
{
	double	dot;
	t_vec3	reflection;

	dot = vec3_dot(vec_in, normal);
	reflection = vec3_sub(vec3_scale(normal, 2 * dot), vec_in);
	return (vec3_norm(reflection));
}

/**
Calculates the specular highlighting coefficient at an intersection point based
on the Phong shading model.
The specular coefficient defines the intensity of the specular highlight,
which contributes to the shiny appearance of a surface.
 @param rt 		Pointer to the main structure.
 @param ix 		Pointer to the intersection data.

 @return		The specular lighting coefficient value [0.0, 1.0].
*/
static double	get_specular_coefficient(t_rt *rt, t_ix *ix)
{
	double	specular;
	double	dot;
	t_vec3	reflection_dir;
	t_vec3	view_dir;

	reflection_dir = get_reflection(ix->light_dir, ix->normal);
	view_dir = vec3_sub(rt->scene.cam.pos, ix->hit_point);
	view_dir = vec3_norm(view_dir);
	dot = vec3_dot(reflection_dir, view_dir);
	if (dot < 0)
		dot = 0;
	specular = pow(dot, K_SHININESS) * rt->scene.light.ratio * K_SPECULAR;
	return (specular);
}

/**
Computes the shading for a pixel based on the object's color and light
properties.
 @param rt	Pointer to the main struct containing light properties.
 @param ix	Pointer to the ray/pixel intersection data.

 @return	A `t_pix_shade` struct with the computed shading components.

 @note		`SPECULAR` is a preprocessor macro (used in 'BONUS' (`make bonus`)).    
 			`FADE` is a preprocessor macro (used in 'BONUS' (`make bonus`)).
*/
t_shade	get_shading(t_rt *rt, t_ix *ix)
{
	t_shade	pix;

	pix.base = ix->hit_obj->color;
	pix.light = rt->scene.light.color;
	pix.ambient = ix->hit_obj->color_in_amb;
	pix.diff_coeff = get_diffuse_coefficient(rt, ix);
	pix.spec_coeff = 0.0;
	if (SPECULAR)
		pix.spec_coeff = get_specular_coefficient(rt, ix);
	pix.fade = 1.0;
	if (FADE)
		pix.fade = clamp(K_FADE * 100 / (ix->light_dist * ix->light_dist), 1.0);
	pix.diffuse.r = pix.base.r * pix.light.r / 255 * pix.diff_coeff * pix.fade;
	pix.diffuse.g = pix.base.g * pix.light.g / 255 * pix.diff_coeff * pix.fade;
	pix.diffuse.b = pix.base.b * pix.light.b / 255 * pix.diff_coeff * pix.fade;
	pix.specular.r = pix.light.r * pix.spec_coeff * pix.fade;
	pix.specular.g = pix.light.g * pix.spec_coeff * pix.fade;
	pix.specular.b = pix.light.b * pix.spec_coeff * pix.fade;
	pix.shaded.r = clamp(pix.ambient.r + pix.diffuse.r + pix.specular.r, 255);
	pix.shaded.g = clamp(pix.ambient.g + pix.diffuse.g + pix.specular.g, 255);
	pix.shaded.b = clamp(pix.ambient.b + pix.diffuse.b + pix.specular.b, 255);
	return (pix);
}

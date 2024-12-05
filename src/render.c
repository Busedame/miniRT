/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:47:07 by aschenk           #+#    #+#             */
/*   Updated: 2024/12/05 19:22:54 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
XXX
*/


#include "main.h"


/**
Function to compute the ray direction for a given pixel in a camera's view,
considering the camera's field of view (FOV) and the aspect ratio of the screen.

This function calculates the direction of the ray in camera space that passes
through a specific pixel on the screen (given by coordinates `x`, `y`).
It accounts for the camera's FOV, the aspect ratio of the screen, and normalizes
the direction vector so that it has a unit length.

 @param x		The x-coordinate of the pixel on the screen (horizontal axis).
 @param y 		The y-coordinate of the pixel on the screen (vertical axis).
 @param cam 	The camera object containing the FOV.

 @return		The normalized direction vector of the ray passing through
				the pixel, in camera space.

 @details
The function works by first converting the pixel coordinates `(x, y)` to
normalized device coordinates in the range of -1 to 1.

This is done by the following steps:
1. Aspect Ratio:	The `aspect_ratio` is calculated by dividing the screen width
					by the screen height. This ensures that the ray direction
					accounts for the shape of the screen.
2. Scaling Factor:	The scaling factor (`scale`) is derived from the cam's FOV.
					The FOV is converted from degrees to radians and then the
					tangent of half the FOV angle is computed. This factor
					scales the pixel's coordinates to match the cam's FOV.
3. Normalized Device Coordinates:	The screen coordinates `(x, y)` are
   transformed into normalized device coordinates (`norm_x`, `norm_y`), where
   the center of the screen is mapped to `(0, 0)` and the edges are mapped to
   `-1` and `1`.
   - `norm_x`: The normalized x-coord., accounting for the aspect ratio and FOV.
   - `norm_y`: The normalized y-coord., scaled by the FOV.
4. Ray Direction:	The `ray_dir` is calculated in camera space as a vector
					`(norm_x, norm_y, 1.0)`. The ray points along the camera's
					forward axis (`z = 1.0`).
5. Normalization:	The `ray_dir` vector is then normalized to ensure it has
					unit length (magnitude = 1).

############################
## STEP-BY-STEP BREAKDOWN ##
############################

*******************************************
** Intermediate Calculations for Scaling **
*******************************************

float fov_half_deg = cam.fov * 0.5; // Half of the field of view in degrees
float fov_half_rad = fov_half_deg * M_PI / 180.0; // Convert degrees to radians
scale = tan(fov_half_rad);  // Calculate the tangent of the half FOV angle

** Halving the Field of View:**
cam.fov * 0.5: The FOV is halved because, in perspective projection, the tangent
function needs the angle from the center of the view to the edge,
not the full angle. This is also known as the vertical half-angle.

** Converting Degrees to Radians:**
M_PI / 180.0: Since trigonometric functions like tan in C/C++ work with radians,
we need to convert the angle from degrees to radians. Multiplying by
π/180 achieves this conversion.

**Tangent Calculation:**
tan(fov_half_rad): Calculates the tangent of the half-angle.
This value represents the relationship between the vertical size of the view at
a certain depth and the distance from the camera. In essence, it defines the
scale factor for mapping the normalized screen coordinates to world coordinates.


*******************************************
** Intermediate Calculations for x and y **
*******************************************

float x_centered = x + 0.5;	// Center of the pixel
float x_normalized = (2.0 * x_centered / WINDOW_W) - 1.0; // Map to [-1, 1]
float x_aspect_corrected = x_normalized * aspect_ratio; // Adjust for aspect r.
norm_x = x_aspect_corrected * scale; // Apply scaling

float y_centered = y + 0.5; // Center of the pixel
float y_normalized = 1.0 - (2.0 * y_centered / WINDOW_H); //	Map [-1, 1] and
																invert Y
norm_y = y_normalized * scale; // Apply scaling

**Centering Pixels:**
`x + 0.5` and `y + 0.5`: This centers the calculation on the pixel's middle
point rather than its corner, improving precision when mapping to continuous
coordinates.

**Normalization to [-1, 1]:**
`(2.0 * (x_centered / WINDOW_W)) - 1.0`: Transforms the range from [0, WINDOW_W]
to [-1, 1]. This standardizes the horizontal coordinates in the NDC.

Similarly,` 1.0 - (2.0 * (y_centered / WINDOW_H))` maps vertical coordinates
from [0, WINDOW_H] to [-1, 1], with the Y-axis inverted so that the top of the
screen is positive.

**Aspect Ratio Correction:**
`x_normalized * aspect_ratio`: Adjusts the X coordinate based on the screen's
aspect ratio to ensure the output doesn't appear stretched.
*/
t_vec3	compute_ray_direction(int x, int y, t_cam cam)
{
	double	aspect_ratio;
	double	scale;
	double	norm_x;
	double	norm_y;
	t_vec3	ray_dir;

	aspect_ratio = (double)WINDOW_W / (double)WINDOW_H;
	scale = tan((cam.fov * 0.5) * M_PI / 180.0);
	norm_x = (2.0 * (x + 0.5) / WINDOW_W - 1.0) * aspect_ratio * scale;
	norm_y = (1.0 - 2.0 * (y + 0.5) / WINDOW_H) * scale;
	ray_dir.x = norm_x;
	ray_dir.y = norm_y;
	ray_dir.z = 1.0;
	return (vec3_norm(ray_dir));
}

/**
Assuming orientation is a normalized forward vector (e.g., camera facing direction)
ou could implement your own rotation logic or use a matrix to rotate vec by orientation.
This is a simplified version using a rotation matrix, depending on your camera's coordinate system.
*/
void rotate_vector_by_orientation(t_vec3 *vec, t_vec3 orientation)
{
    // Assuming 'orientation' is the camera's forward vector (normalized)
    double pitch = asin(orientation.y);  // Pitch = arcsin(orientation.y)
    double yaw = atan2(orientation.x, orientation.z);  // Yaw = atan2(x, z)

    // Rotation matrix components for pitch and yaw
    double cos_pitch = cos(pitch);
    double sin_pitch = sin(pitch);
    double cos_yaw = cos(yaw);
    double sin_yaw = sin(yaw);

    // Apply the rotation matrix
    double x_new = vec->x * cos_yaw - vec->z * sin_yaw;
    double y_new = vec->x * sin_pitch * sin_yaw + vec->y * cos_pitch - vec->z * sin_pitch * cos_yaw;
    double z_new = vec->x * cos_pitch * sin_yaw + vec->y * sin_pitch * cos_yaw + vec->z * cos_pitch * cos_yaw;

    // Update the original vector with the rotated result
    vec->x = x_new;
    vec->y = y_new;
    vec->z = z_new;
}

void render_scene(t_rt *rt, int bg_color)
{
    int x, y;
    t_vec3 ray_origin;
    t_vec3 ray_dir;
    double t, closest_t;
    int pixel_color;
    t_list *current_obj;
    t_obj_data *obj_data;

    // Loop through each pixel
    for (y = 0; y < WINDOW_H; y++)
    {
        for (x = 0; x < WINDOW_W; x++)
        {
            // Compute the direction of the ray for this pixel
            ray_origin.x = rt->scene.cam.position.x;
            ray_origin.y = rt->scene.cam.position.y;
            ray_origin.z = rt->scene.cam.position.z;

            ray_dir = compute_ray_direction(x, y, rt->scene.cam);

            // Initialize hit to false and pixel color to background color
            pixel_color = bg_color;
            closest_t = INFINITY;

            // Iterate over all objects in the scene
            current_obj = rt->scene.objs;
            while (current_obj != NULL)
            {
                obj_data = (t_obj_data *)current_obj->content;

                // Check for intersection with plane
                if (obj_data->pl.object_type == PLANE)
                {
                    if (ray_intersect_plane(ray_dir, &obj_data->pl, &t))
                    {
                        // If a hit occurs and it's closer than the previous hit, update the pixel color
                        if (t < closest_t)
                        {
                            closest_t = t;
                            pixel_color = color_to_hex(obj_data->pl.color);
                        }
                    }
                }

                // Check for intersection with sphere
                if (obj_data->sp.object_type == SPHERE)
                {
                    if (ray_intersect_sphere(ray_dir, &obj_data->sp, &t))
                    {
                        // If a hit occurs and it's closer than the previous hit, update the pixel color
                        if (t < closest_t)
                        {
                            closest_t = t;
                            pixel_color = color_to_hex(obj_data->sp.color);
                        }
                    }
                }

                // Check for intersection with sphere
                if (obj_data->cy.object_type == CYLINDER)
                {
                    if (ray_intersect_cylinder(ray_origin, ray_dir, &obj_data->cy, &t))
                    {
                        // If a hit occurs and it's closer than the previous hit, update the pixel color
                        //printf("Cylinder hit\n");
                        if (t < closest_t)
                        {
                            closest_t = t;
                            pixel_color = color_to_hex(obj_data->cy.color);
                        }
                    }
                    if (obj_data->cy.ixd.cap_hit != 2 && ray_intersect_cap_top(ray_origin, ray_dir, &obj_data->cy, &t))
                    {
                        // If a hit occurs and it's closer than the previous hit, update the pixel color
                      //  printf("Top cap hit\n");
                        if (t < closest_t)
                        {
                            closest_t = t;
                            pixel_color = color_to_hex(obj_data->cy.color);
                        }
                        if (obj_data->cy.ixd.cap_hit == 0)
                            obj_data->cy.ixd.cap_hit = 1;
                    }
                    if (obj_data->cy.ixd.cap_hit != 1 && ray_intersect_cap_bottom(ray_origin, ray_dir, &obj_data->cy, &t))
                    {
                        // If a hit occurs and it's closer than the previous hit, update the pixel color
                       // printf("Bottom cap hit\n");
                        if (t < closest_t)
                        {
                            closest_t = t;
                            pixel_color = color_to_hex(obj_data->cy.color);
                        }
                        if (obj_data->cy.ixd.cap_hit == 0)
                            obj_data->cy.ixd.cap_hit = 2;
                    }
                }

                // Move to the next object in the scene
                current_obj = current_obj->next;
            }

            // Set pixel color using the set_pixel_color function
            set_pixel_color(&rt->mlx.img, x, y, pixel_color);
        }
    }
}

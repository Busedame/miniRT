/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 18:13:03 by aschenk           #+#    #+#             */
/*   Updated: 2024/12/05 19:20:11 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/**
Function to create a simple scene with a three sphere.
Takes a pointer to t_rt, which includes both mlx and scene.
*/
void	create_simple_scene(t_rt *rt)
{
    t_list		*obj_node;
    t_obj_data	*obj_data;

    // Set up ambient light
    rt->scene.ambi_light.ratio = 0.1;  // Low ambient light
    rt->scene.ambi_light.color.r = 255;
    rt->scene.ambi_light.color.g = 255;
    rt->scene.ambi_light.color.b = 255;

    // Set up the light source
    rt->scene.light.position.x = 0.0;
    rt->scene.light.position.y = 10.0;
    rt->scene.light.position.z = 0.0;
    rt->scene.light.ratio = 1.0;  // Full brightness

    // Set up the camera
    rt->scene.cam.position.x = 1.0;
    rt->scene.cam.position.y = 1.0;
    rt->scene.cam.position.z = -5.0;  // Camera placed along the negative Z-axis
    rt->scene.cam.orientation.x = 0.0;
    rt->scene.cam.orientation.y = 0.0;
    rt->scene.cam.orientation.z = 1.0;  // Looking down the Z-axis
    rt->scene.cam.fov = 90.0;  // Field of view

    // Set up a single sphere object (front)
    obj_data = malloc(sizeof(t_obj_data));
    if (!obj_data)
    {
        perror("Failed to allocate memory for obj_data");
        return;
    }

    // Create the sphere data
    obj_data->sp.object_type = SPHERE;
    obj_data->sp.center.x = 1.0;
    obj_data->sp.center.y = 0.0;
    obj_data->sp.center.z = 0.0;
    obj_data->sp.radius = 2.0;  // Radius of the sphere
    obj_data->sp.color.r = 200;
    obj_data->sp.color.g = 0;
    obj_data->sp.color.b = 200;  // Red sphere
    // Intialize constant values
    obj_data->sp.ixd.oc = vec3_sub(rt->scene.cam.position, obj_data->sp.center);
    obj_data->sp.ixd.c = vec3_dot(obj_data->sp.ixd.oc, obj_data->sp.ixd.oc) - (obj_data->sp.radius * obj_data->sp.radius);

    // Create a new list node with the sphere data
    obj_node = ft_lstnew(obj_data);
    if (!obj_node)
    {
        free(obj_data);  // Clean up if node creation fails
        perror("Failed to create list node for sphere");
        return;  // Handle memory allocation failure
    }

    ft_lstadd_front(&rt->scene.objs, obj_node);

    // Set up the cylinder object (blue, replacing the second sphere)
    obj_data = malloc(sizeof(t_obj_data));
    if (!obj_data)
    {
     perror("Failed to allocate memory for obj_data");
     return;
    }

    obj_data->cy.object_type = CYLINDER;
    obj_data->cy.center.x = 0.0;  // Shift closer to the camera
    obj_data->cy.center.y = 1.0;
    obj_data->cy.center.z = -0.50;
    obj_data->cy.orientation.x = 0.0;
    obj_data->cy.orientation.y = 1.0;  // Aligned along Y-axis
    obj_data->cy.orientation.z = 0.0;
    obj_data->cy.radius = 1.0;  // Radius of the cylinder
    obj_data->cy.radius_sqrd = obj_data->cy.radius * obj_data->cy.radius;
    obj_data->cy.height = 6.0;  // Height of the cylinder
    obj_data->cy.color.r = 255;
    obj_data->cy.color.g = 0;
    obj_data->cy.color.b = 0;  // Blue cylinder

    // Initialize the quadratic coefficients that are constant (not dependent on the ray)
    obj_data->cy.ixd.cap_hit = 0;
    obj_data->cy.cap_top_center = vec3_add(obj_data->cy.center, vec3_mult(obj_data->cy.orientation, obj_data->cy.height / 2.0));
    obj_data->cy.cap_bottom_center = vec3_sub(obj_data->cy.center, vec3_mult(obj_data->cy.orientation, obj_data->cy.height / 2.0));

    obj_data->cy.cap_top_normal = obj_data->cy.orientation;
    obj_data->cy.cap_bottom_normal = vec3_mult(obj_data->cy.orientation, -1.0);

    obj_data->cy.ixd.oc = vec3_sub(rt->scene.cam.position, obj_data->cy.center);
    obj_data->cy.ixd.axis_dot_oc = vec3_dot(obj_data->cy.ixd.oc, obj_data->cy.orientation);
    obj_data->cy.ixd.c = vec3_dot(obj_data->cy.ixd.oc, obj_data->cy.ixd.oc)
        - (obj_data->cy.ixd.axis_dot_oc * obj_data->cy.ixd.axis_dot_oc)
        - obj_data->cy.radius_sqrd;
    obj_data->cy.ixd.ori_to_cap_top = vec3_sub(obj_data->cy.cap_top_center, rt->scene.cam.position);
    obj_data->cy.ixd.ori_to_cap_bottom = vec3_sub(obj_data->cy.cap_bottom_center, rt->scene.cam.position);

    obj_node = ft_lstnew(obj_data);
    if (!obj_node)
    {
        free(obj_data);  // Clean up if node creation fails
        perror("Failed to create list node for cylinder");
        return;
    }

    ft_lstadd_front(&rt->scene.objs, obj_node);

    // Set up the third sphere object (green, in front of the red one)
    obj_data = malloc(sizeof(t_obj_data));
    if (!obj_data)
    {
        perror("Failed to allocate memory for obj_data");
        return;
    }

    obj_data->sp.object_type = SPHERE;
    obj_data->sp.center.x = 0.5;
    obj_data->sp.center.y = 1.0;
    obj_data->sp.center.z = -0.5;  // Positioned in front of the red sphere
    obj_data->sp.radius = 1.5;  // Radius of the sphere
    obj_data->sp.color.r = 0;
    obj_data->sp.color.g = 255;
    obj_data->sp.color.b = 0;  // Green sphere
    // Initialie constant values
    obj_data->sp.ixd.oc = vec3_sub(rt->scene.cam.position, obj_data->sp.center);
    obj_data->sp.ixd.c = vec3_dot(obj_data->sp.ixd.oc, obj_data->sp.ixd.oc) - (obj_data->sp.radius * obj_data->sp.radius);

    obj_node = ft_lstnew(obj_data);
    if (!obj_node)
    {
        free(obj_data);  // Clean up if node creation fails
        perror("Failed to create list node for sphere");
        return;  // Handle memory allocation failure
    }

    ft_lstadd_front(&rt->scene.objs, obj_node);

    // Set up the floor plane (horizontal plane)
    obj_data = malloc(sizeof(t_obj_data));
    if (!obj_data)
    {
        perror("Failed to allocate memory for obj_data");
        return;
    }

    obj_data->pl.object_type = PLANE;
    obj_data->pl.point_in_plane.x = 0.0;
    obj_data->pl.point_in_plane.y = 0.0;  // Positioned below the spheres
    obj_data->pl.point_in_plane.z = 0.0;  // Parallel to XZ plane
    obj_data->pl.normal.x = 0.0;
    obj_data->pl.normal.y = 1.0;  // Normal facing upwards
    obj_data->pl.normal.z = 0.0;
    obj_data->pl.color.r = 200;  // Light grey plane
    obj_data->pl.color.g = 200;
    obj_data->pl.color.b = 200;
    // Initalize constant plane values
    obj_data->pl.ixd.difference = vec3_sub(obj_data->pl.point_in_plane, rt->scene.cam.position);

    obj_node = ft_lstnew(obj_data);
    if (!obj_node)
    {
        free(obj_data);
        perror("Failed to create list node for plane");
        return;
    }
    ft_lstadd_front(&rt->scene.objs, obj_node);

    obj_data = malloc(sizeof(t_obj_data));
    if (!obj_data)
    {
        perror("Failed to allocate memory for obj_data");
        return;
    }

    obj_data->cy.object_type = CYLINDER;
    obj_data->cy.center.x = 0.0;  // Centered in front of the camera
    obj_data->cy.center.y = 0.0;
    obj_data->cy.center.z = 0.0;  // Position along Z-axis for better view
    obj_data->cy.orientation.x = 0.0;
    obj_data->cy.orientation.y = 0.0;
    obj_data->cy.orientation.z = 1.0;  // Oriented along Z-axis
    obj_data->cy.radius = 3.5;  // Large enough for the camera to see through
    obj_data->cy.radius_sqrd = obj_data->cy.radius * obj_data->cy.radius;
    obj_data->cy.height = 3.0;  // Long enough to encompass the FOV
    obj_data->cy.color.r = 100;
    obj_data->cy.color.g = 100;
    obj_data->cy.color.b = 255;  // Light blue cylinder interior

    // Initialize the quadratic coefficients that are constant (not dependent on the ray)
    obj_data->cy.ixd.cap_hit = 0;
    obj_data->cy.cap_top_center = vec3_add(obj_data->cy.center, vec3_mult(obj_data->cy.orientation, obj_data->cy.height / 2.0));
    obj_data->cy.cap_bottom_center = vec3_sub(obj_data->cy.center, vec3_mult(obj_data->cy.orientation, obj_data->cy.height / 2.0));
    obj_data->cy.cap_top_normal = obj_data->cy.orientation;
    obj_data->cy.cap_bottom_normal = vec3_mult(obj_data->cy.orientation, -1.0);
    obj_data->cy.ixd.oc = vec3_sub(rt->scene.cam.position, obj_data->cy.center);
    obj_data->cy.ixd.axis_dot_oc = vec3_dot(obj_data->cy.ixd.oc, obj_data->cy.orientation);
    obj_data->cy.ixd.c = vec3_dot(obj_data->cy.ixd.oc, obj_data->cy.ixd.oc)
        - (obj_data->cy.ixd.axis_dot_oc * obj_data->cy.ixd.axis_dot_oc)
        - obj_data->cy.radius_sqrd;
    obj_data->cy.ixd.ori_to_cap_top = vec3_sub(obj_data->cy.cap_top_center, rt->scene.cam.position);
    obj_data->cy.ixd.ori_to_cap_bottom = vec3_sub(obj_data->cy.cap_bottom_center, rt->scene.cam.position);

    obj_node = ft_lstnew(obj_data);
    if (!obj_node)
    {
        free(obj_data);
        perror("Failed to create list node for cylinder");
        return;
    }
    ft_lstadd_front(&rt->scene.objs, obj_node);

    printf("Scene created\n");
}

int	main(int argc, char **argv)
{
	t_rt	*rt;
	//t_scene	scene;

	rt = ft_calloc(1, sizeof(t_rt));
	if (!rt)
		cleanup_error_exit(ERR_MALLOC, NULL);

    (void)argc;
    (void)argv;
   // scene = parse_and_set_objects(argc, argv);

    create_simple_scene(rt);

	init_mlx(rt);

	render_scene(rt, BLACK); // render a simple scene without shadows, lighting considerations etc.

	//fill_image(rt, RED); // just for testing to see if image shown in window

	mlx_put_image_to_window(rt->mlx.mlx_ptr, rt->mlx.win_ptr,
		rt->mlx.img.img_ptr, 0, 0);

	start_event_loop(rt);
}

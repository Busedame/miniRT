/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 18:13:07 by aschenk           #+#    #+#             */
/*   Updated: 2024/11/22 18:34:31 by aschenk          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

// Standard C libraries
# include <unistd.h>		// for open(), close(), read(), write()
# include <stdio.h>			// for printf(), perror()
# include <string.h>		// for strerror()
# include <stdlib.h>		// for malloc(), free(), exit()
# include <errno.h>			// for errno
# include <math.h>			// for math functions, expr/ , e.g. sqrt(), INFINITY

// X11 library headers
# include <X11/keysym.h>	// Macros for key symbols, e.g. XK_Escape
# include <X11/X.h>			// Macros for event names, e.g. DestroyNotify

// Custom headers
# include "libft.h"			// libft
# include "mlx.h"			// mlx
# include "errors.h"		// Error messages and formatting styles
# include "types.h"			// Typedefs, data structs, e.g. 't_rt'
# include "colors.h"

//#############
//# CONSTANTS #
//#############

# define WINDOW_TITLE	"miniRT by Natalie Holbrook & Alex Schenk @42Berlin"

//##################
//# FCT PROTOTYPES #
//##################

// initialization.c

void	init_mlx(t_rt *rt);

// event_handlers.c

void	start_event_loop(t_rt *rt);

// utils/cleanup.c

void	cleanup(t_rt **rt_ptr);

// utils/error_exit.c

void	cleanup_error_exit(char *msg, t_rt *rt);

// utils/vector_utils.c

t_vec3	vec3_sub(t_vec3 v1, t_vec3 v2);
double	vec3_dot(t_vec3 v1, t_vec3 v2);
t_vec3	vec3_scale(t_vec3 v, double scalar);

// utils/mlx_utils.c

int		color_to_hex(t_color color);
void	set_pixel_color(t_img *img, int x, int y, int color);
void	fill_image(t_rt *rt, int color); // TESTING ONLY
void	draw_colored_pixels(t_rt *rt, int color); // TESTING ONLY

void	render_scene(t_rt *rt, int bg_color);


#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"
#include "ei_implementation.h"

/* test_star --
 *
 *	Draws a star in the middle of the screen.
 */
void test_circle(ei_surface_t surface, ei_point_t centre, int rayon, ei_color_t color, ei_rect_t* clipper)
{
	int tab_size = ei_octant_array_size(rayon);

	for (int octant = 0; octant < 8; octant++) {
		ei_point_t *pts = ei_octant(centre, rayon, octant);
		
		for (int i = 0; i < tab_size; i++) {
			ei_fill_pixel(surface, &color, pts[i]);
		}

		free(pts);
	}
}


/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;
	ei_color_t		white		= { 255, 255, 255, 255 };
	ei_color_t		black		= { 0, 0, 0, 255 };
	ei_color_t		red		= { 255, 0, 0, 255 };
	ei_color_t		dark_red	= { 100, 0, 0, 0 };
	ei_event_t		event;
	ei_rect_t 		rect 		= ei_rect(ei_point(100, 100), ei_size(300, 200));

	hw_init();

	main_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);

	ei_point_t centre = { 600, 400 };
	int rayon = 50;

	/* Draw octants. */
	ei_fill(main_window, &white, NULL);
	test_circle(main_window, centre, rayon, black, NULL);

	/* Draw button. */
	int octant_array_size = ei_octant_array_size(rayon);

	ei_point_t *top = ei_half_rounded_frame(rect, rayon, TOP);
	ei_draw_polygon(main_window, top, 4 * octant_array_size + 2, red, NULL);
	free(top);

	ei_point_t *bottom = ei_half_rounded_frame(rect, rayon, BOTTOM);
	ei_draw_polygon(main_window, bottom, 4 * octant_array_size + 2, dark_red, NULL);
	free(bottom);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

	/* Wait for a character on command line. */
	event.type = ei_ev_none;
	while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
		hw_event_wait_next(&event);

	hw_quit();
	hw_surface_free(main_window);
	return (EXIT_SUCCESS);
}
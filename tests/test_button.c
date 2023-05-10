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
	ei_point_t *pts1 = ei_octant(centre, rayon, 0);
	ei_point_t *pts2 = ei_octant(centre, rayon, 1);
	ei_point_t *pts3 = ei_octant(centre, rayon, 2);
	ei_point_t *pts4 = ei_octant(centre, rayon, 3);
	ei_point_t *pts5 = ei_octant(centre, rayon, 4);
	ei_point_t *pts6 = ei_octant(centre, rayon, 5);
	ei_point_t *pts7 = ei_octant(centre, rayon, 6);
	ei_point_t *pts8 = ei_octant(centre, rayon, 7);


	/* Draw pixels */
	for (int i = 0; i < tab_size; i++) {
		ei_fill_pixel(surface, &color, pts1[i]);
		ei_fill_pixel(surface, &color, pts2[i]);
		ei_fill_pixel(surface, &color, pts3[i]);
		ei_fill_pixel(surface, &color, pts4[i]);
		ei_fill_pixel(surface, &color, pts5[i]);
		ei_fill_pixel(surface, &color, pts6[i]);
		ei_fill_pixel(surface, &color, pts7[i]);
		ei_fill_pixel(surface, &color, pts8[i]);
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
	ei_color_t		red		= { 255, 0, 0, 255 };
	ei_color_t		dark_red		= { 100, 0, 0, 0 };
	ei_event_t		event;
	ei_rect_t 		rect 		= (ei_rect_t) {(ei_point_t) {100, 100}, (ei_size_t) {300, 200}};

	hw_init();

	main_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);

	ei_point_t centre = (ei_point_t) { 300, 200};
	int rayon = 50;

	/* Draw octant. */
	ei_fill(main_window, &white, NULL);
	test_circle(main_window, centre, rayon, white, NULL);

	ei_point_t *top = ei_rounded_frame(rect, rayon,TOP);
	ei_point_t *bottom = ei_rounded_frame(rect, rayon,BOTTOM);
	int octant_array_size = ei_octant_array_size(rayon);
	ei_draw_polygon(main_window, top, 4*octant_array_size+2, red, NULL);
	ei_draw_polygon(main_window, bottom, 4*octant_array_size+2, dark_red, NULL);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

	/* Wait for a character on command line. */
	event.type = ei_ev_none;
	while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
		hw_event_wait_next(&event);

	hw_quit();
	return (EXIT_SUCCESS);
}
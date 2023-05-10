#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"



/* test_star --
 *
 *	Draws a star in the middle of the screen.
 */
void test_star(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_point_t		pts[10];

	pts[0].x = 184+215;
	pts[0].y = -175+420;
	pts[1].x = 169+215;
	pts[1].y = -138+420;
	pts[2].x = 129+215;
	pts[2].y = -138+420;
	pts[3].x = 162+215;
	pts[3].y = -112+420;
	pts[4].x = 150+215;
	pts[4].y = -76+420;
	pts[5].x = 182+215;
	pts[5].y = -97+420;
	pts[6].x = 217+215;
	pts[6].y = -76+420;
	pts[7].x = 205+215;
	pts[7].y = -111+420;
	pts[8].x = 237+215;
	pts[8].y = -138+420;
	pts[9].x = 198+215;
	pts[9].y = -138+420;

	/* Draw the shape with a polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), (ei_color_t){ 57, 130, 71, 255 }, clipper);
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
	ei_color_t		green		= { 57, 130, 71, 255 };
	ei_color_t		yellow		= { 251, 238, 101, 255 };
	ei_color_t		red		= { 209, 53, 48, 255 };
	ei_rect_t		clipper_green	= ei_rect(ei_point(200, 150), ei_size(120, 300));
	ei_rect_t		clipper_yellow	= ei_rect(ei_point(200+120, 150), ei_size(160, 300));
	ei_rect_t		clipper_red	= ei_rect(ei_point(200+280, 150), ei_size(120, 300));
	ei_event_t		event;

	hw_init();

	main_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	ei_fill(main_window, &green, &clipper_green);
	ei_fill(main_window, &yellow, &clipper_yellow);
	ei_fill(main_window, &red, &clipper_red);

	/* Draw polylines. */
	test_star(main_window, &clipper_yellow);

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

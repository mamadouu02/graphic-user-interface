#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"


/* test_star_morocco
 *
 *	Draws a star in the middle of the screen.
 */
void test_star_morocco(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_point_t		pts[6];
	ei_point_t		pts1[6];
	ei_point_t		pts2[6];

	pts[0].x = 184+215;
	pts[0].y = -175+420;
	pts[1].x = 150+215;
	pts[1].y = -76+420;
	pts[2].x = 237+215;
	pts[2].y = -138+420;
	pts[3].x = 129+215;
	pts[3].y = -138+420;
	pts[4].x = 217+215;
	pts[4].y = -76+420;
	pts[5].x = 184+215;
	pts[5].y = -175+420;


	pts1[0].x = 184+215-1;
	pts1[0].y = -175+420;
	pts1[1].x = 150+215-1;
	pts1[1].y = -76+420;
	pts1[2].x = 237+215-1;
	pts1[2].y = -138+420;
	pts1[3].x = 129+215-1;
	pts1[3].y = -138+420;
	pts1[4].x = 217+215-1;
	pts1[4].y = -76+420;
	pts1[5].x = 184+215-1;
	pts1[5].y = -175+420;


	pts2[0].x = 184+215+1;
	pts2[0].y = -175+420;
	pts2[1].x = 150+215+1;
	pts2[1].y = -76+420;
	pts2[2].x = 237+215+1;
	pts2[2].y = -138+420;
	pts2[3].x = 129+215+1;
	pts2[3].y = -138+420;
	pts2[4].x = 217+215+1;
	pts2[4].y = -76+420;
	pts2[5].x = 184 +215+1;
	pts2[5].y = -175+420; 

	ei_draw_polyline(surface, pts , 6, (ei_color_t){0, 98, 48, 10}, clipper);
	ei_draw_polyline(surface, pts1 , 6, (ei_color_t){0, 98, 48, 10}, clipper);
	ei_draw_polyline(surface, pts2 , 6, (ei_color_t){0, 98, 48, 10}, clipper);
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
	ei_color_t		white		= { 0xff, 0xff, 0xff, 0xff };
	ei_color_t		color		= { 194, 34, 41, 255 };
	ei_rect_t*		clipper_ptr	= NULL;
	ei_rect_t		clipper		= ei_rect(ei_point(200, 150), ei_size(400, 300));
	clipper_ptr		= &clipper;
	ei_event_t		event;

	hw_init();

	main_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	ei_fill		(main_window, &white, NULL);

	/* Fill the flag in red. */
	ei_fill(main_window, &color, clipper_ptr);

	/* Draw the moroccan star. */
	test_star_morocco(main_window, clipper_ptr);

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


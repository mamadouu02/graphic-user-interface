#include <stdlib.h>
#include <stdio.h>

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
void test_star_senegal(ei_surface_t surface, ei_rect_t* clipper)
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



void test_star_tunisia(ei_surface_t surface, ei_rect_t* clipper, ei_color_t color)
{
	ei_point_t		pts[10];

	pts[0].x = 439;
	pts[0].y = 316;
	pts[1].x = 426;
	pts[1].y = 310;
	pts[2].x = 402;
	pts[2].y = 325;
	pts[3].x = 407;
	pts[3].y = 308;
	pts[4].x = 387;
	pts[4].y = 301;
	pts[5].x = 407;
	pts[5].y = 295;
	pts[6].x = 409;
	pts[6].y = 275;
	pts[7].x = 420;
	pts[7].y = 292;
	pts[8].x = 440;
	pts[8].y = 288;
	pts[9].x = 427;
	pts[9].y = 303;

	/* Draw the shape with a polygon */
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), color, clipper);
}


void test_circle(ei_surface_t surface, ei_point_t centre, int rayon, ei_color_t color, ei_rect_t* clipper)
{
	int octant_array_size = ei_octant_array_size(rayon);
	ei_point_t *circle = malloc(sizeof(ei_point_t[8*octant_array_size]));
	int tour = 0;

	for (int octant = 0; octant < 8; octant++) {
		ei_point_t *pts = ei_octant(centre, rayon, octant, octant_array_size);

		for (int i = tour*octant_array_size ; i < (tour+1)*octant_array_size; i++) {
			circle[i] = pts[i - tour*octant_array_size];
		}
		tour++;
		free(pts);
	}
	ei_draw_polygon(surface, circle, 8*octant_array_size, color, clipper);
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(void)
{
	ei_size_t		win_size		= ei_size(800, 600);
	ei_surface_t		main_window		= NULL;
	ei_surface_t		second_window		= NULL;
	ei_color_t		white			= { 255, 255, 255, 255 };
	ei_color_t		green			= { 57, 130, 71, 255 };
	ei_color_t		yellow			= { 251, 238, 101, 255 };
	ei_color_t		red			= { 209, 53, 48, 255 };
	ei_rect_t		clipper_green		= ei_rect(ei_point(200, 150), ei_size(120, 300));
	ei_rect_t		clipper_yellow		= ei_rect(ei_point(200+120, 150), ei_size(160, 300));
	ei_rect_t		clipper_red_senegal	= ei_rect(ei_point(200+280, 150), ei_size(120, 300));
	ei_rect_t		clipper_red_tunisia	= ei_rect(ei_point(200, 150), ei_size(400, 300));
	ei_event_t		event;

	hw_init();

	main_window = hw_create_window(win_size, false);
	second_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	hw_surface_lock	(second_window);

	/* Fill windows for Senegal and Tunisia */
	ei_fill(second_window, &green, &clipper_green);
	ei_fill(second_window, &yellow, &clipper_yellow);
	ei_fill(second_window, &red, &clipper_red_senegal);
	ei_fill(main_window, &red, &clipper_red_tunisia);

	/* Draw filled circles for Tunisia. */
	ei_point_t centre_first = ei_point(400, 300);
	int rayon_first = 70;
	test_circle(main_window, centre_first, rayon_first, white, NULL);

	ei_point_t centre_second = ei_point(400, 300);
	int rayon_second = 52;
	test_circle(main_window, centre_second, rayon_second, red, NULL);

	ei_point_t centre_third = ei_point(415, 300);
	int rayon_third = 42;
	test_circle(main_window, centre_third, rayon_third, white, NULL);


	/* Draw the two stars. */
	test_star_senegal(second_window, &clipper_yellow);
	test_star_tunisia(main_window, NULL, red);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_unlock(second_window);
	hw_surface_update_rects(main_window, NULL);
	hw_surface_update_rects(second_window, NULL);

	/* Wait for a character on command line. */
	event.type = ei_ev_none;
	while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
		hw_event_wait_next(&event);

	hw_quit();
	hw_surface_free(main_window);
	hw_surface_free(second_window);
	return (EXIT_SUCCESS);
}

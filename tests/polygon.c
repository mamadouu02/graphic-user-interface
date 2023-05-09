//
// Created by akliaib on 09/05/2023.
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hw_interface.h"
#include "ei_utils.h"
#include "ei_draw.h"
#include "ei_types.h"
#include "ei_event.h"




/* test_octogone --
 *
 *	Draws an octogone in the middle of the screen. This is meant to test the
 *	algorithm that draws a polyline in each of the possible octants, that is,
 *	in each quadrant with dx>dy (canonical) and dy>dx (steep).
 */


void test_star_morocco(ei_surface_t surface, ei_rect_t* clipper)
{
//	ei_point_t		pts[10];
	ei_point_t		pts[6];
	ei_point_t		pts1[6];
	ei_point_t		pts2[6];
	/* Initialisation */


//	C=(183.698143561132,175.3554626423045);
//	D=(169.6867735769101,138.1714422995618);
//	E=(129.2693601608854,137.6325434540148);
//	F=(161.6032908937051,111.765398867759);
//	G=(149.7475162916712,75.6591762161103);
//	H=(182.081447024491,96.6762311924432);
//	I=(216.5709731394987,74.5813785250163);
//	J=(204.7151985374648,111.226500022212);
//	K=(237.0491292702846,136.5547457629209);
//	L=(197.7095135453539,136.0158469173738);

//	pts[0].x = 184+215;
//	pts[0].y = -175+420;
//	pts[1].x = 169+215;
//	pts[1].y = -138+420;
//	pts[2].x = 129+215;
//	pts[2].y = -138+420;
//	pts[3].x = 162+215;
//	pts[3].y = -112+420;
//	pts[4].x = 150+215;
//	pts[4].y = -76+420;
//	pts[5].x = 182+215;
//	pts[5].y = -97+420;
//	pts[6].x = 217+215;
//	pts[6].y = -76+420;
//	pts[7].x = 205+215;
//	pts[7].y = -111+420;
//	pts[8].x = 237+215;
//	pts[8].y = -138+420;
//	pts[9].x = 198+215;
//	pts[9].y = -138+420;

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
	pts2[5].x = 184+215+1;
	pts2[5].y = -175+420;

	ei_draw_polyline(surface,pts,6,(ei_color_t){0, 98, 48, 10},clipper);
	ei_draw_polyline(surface,pts1,6,(ei_color_t){0, 98, 48, 10},clipper);
	ei_draw_polyline(surface,pts2,6,(ei_color_t){0, 98, 48, 10},clipper);

}

void test_star(ei_surface_t surface, ei_rect_t* clipper)
{
//	ei_point_t		pts[10];
	ei_point_t		pts[10];
	/* Initialisation */

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
	ei_draw_polygon(surface, pts, sizeof(pts) / sizeof(ei_point_t), (ei_color_t){0, 98, 48, 10}, clipper);

}



/* test_square --
 *
 *	Draws a square in the middle of the screen. This is meant to test the
 *	algorithm for the special cases of horizontal and vertical lines, where
 *	dx or dy are zero
 */
void test_square(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 194,34,41, 255 };
	ei_point_t		pts[5];
	int			i, xdiff, ydiff;

	/* Initialisation */
	pts[0].x		= 300;
	pts[0].y		= 400;

	/* Draw the square */
	for(i = 1; i <= 4; i++) {
		/*	Add or remove 200 pixels or 0 for next point
			The first term of this formula gives the sign + or - of the operation
			The second term is 0 or 1, according to which coordinate grows
			The third term is simply the side of the square */
		xdiff = (int)(pow(-1, i / 2) * (i % 2) * 200);
		ydiff = (int)(pow(-1, i / 2) * (i % 2 == 0) * 200);

		pts[i].x	= pts[i-1].x + xdiff;
		pts[i].y	= pts[i-1].y + ydiff;
	}

	/* Draw the shape with polylines */
	ei_draw_polyline(surface, pts, sizeof(pts) / sizeof(ei_point_t), color, clipper);
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
	ei_color_t		color		= { 194,34,41, 255 };
	ei_rect_t*		clipper_ptr	= NULL;
	ei_rect_t		clipper		= ei_rect(ei_point(200, 150), ei_size(400, 300));
	clipper_ptr		= &clipper;
	ei_event_t		event;

	hw_init();

	main_window = hw_create_window(win_size, false);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
	ei_fill		(main_window, &white, NULL);
	ei_fill(main_window,&color,clipper_ptr);

	/* Draw polylines. */

	test_square	(main_window, clipper_ptr);
	test_star(main_window, clipper_ptr);
	test_star_morocco(main_window, clipper_ptr);



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


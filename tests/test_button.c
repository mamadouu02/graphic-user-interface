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
	int octant_array_size = ei_octant_array_size(rayon);

	for (int octant = 0; octant < 8; octant++) {
		ei_point_t *pts = ei_octant(centre, rayon, octant, octant_array_size);
		
		for (int i = 0; i < octant_array_size; i++) {
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
	ei_surface_t	main_window = NULL;
	ei_surface_t	second_window = NULL;
	ei_color_t		white		= { 255, 255, 255, 255 };
	ei_color_t		black		= { 0, 0, 0, 255 };
	ei_color_t		red		= { 255, 0, 0, 255 };
	ei_color_t		belle		= { 164, 122, 84, 255 };
	ei_color_t		dark_red	= { 100, 0, 0, 0 };
	ei_color_t 		beige		= { 236, 207, 202, 0 };
	ei_event_t		event;
	ei_rect_t 		rect 		= ei_rect(ei_point(100, 100), ei_size(300, 200));
//	ei_rect_t 		rect_petit 		= ei_rect(ei_point(100, 299), ei_size(300, 200));
//	ei_rect_t 		rect_copy 		= ei_rect(ei_point(100, 350), ei_size(300, 200));
	ei_rect_t 		rect_button 		= ei_rect(ei_point(450, 100), ei_size(300, 200));
//	ei_rect_t		clipper		= ei_rect(ei_point(555, 180), ei_size(15, 15));;
	ei_rect_t		*clipper		= NULL;
	hw_init();

	main_window = hw_create_window(win_size, false);
//	second_window = hw_create_window(win_size, false);
	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
//	hw_surface_lock	(second_window);

	ei_point_t centre = { 600, 400 };
	int rayon = 50;

	/* Draw octants. */
	ei_fill(main_window, &white, NULL);
	ei_fill(main_window, &belle, clipper);
	test_circle(main_window, centre, rayon, black, NULL);

	/* Draw button. */
	int octant_array_size = ei_octant_array_size(rayon);

	ei_point_t *top = ei_rounded_frame(rect, rayon, TOP);
	ei_draw_polygon(main_window, top, 4 * octant_array_size + 2, red, clipper);
	free(top);

	ei_point_t *bottom = ei_rounded_frame(rect, rayon, BOTTOM);
	ei_draw_polygon(main_window, bottom, 4 * octant_array_size + 2, dark_red, clipper);
	free(bottom);

	draw_button(main_window, rect_button, beige, clipper);

	/* Test intersection */
//	ei_rect_t rect_total = ei_rect(ei_point(0, 0), win_size);
//	ei_rect_t inter = rect_intersection(rect_total, rect);
//	ei_point_t top_left = inter.top_left;
//	ei_point_t top_right = (ei_point_t) {top_left.x+inter.size.width, top_left.y};
//	ei_point_t bottom_left = (ei_point_t) {top_left.x, top_left.y + inter.size.height};
//	ei_point_t bottom_right = (ei_point_t) {top_left.x + inter.size.width, top_left.y + inter.size.height};
//	ei_point_t point_array[4] = { top_left, top_right, bottom_right, bottom_left };
//	ei_draw_polygon(second_window, point_array, 4, red, NULL);

	/* Copy surface */
//	ei_copy_surface(second_window, &rect_petit, main_window,&rect , false);

	/* Draw text */
	ei_point_t where = ei_point(0,0);
	ei_const_string_t text = "Button";

	int font_size = rect.size.height/6;
	ei_const_string_t font_filename = "misc/font.ttf";
	ei_font_t font = hw_text_font_create(font_filename,ei_style_normal, font_size);

	ei_draw_text(main_window, &where, text, font, black, NULL);
	hw_text_font_free(font);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);
	hw_surface_unlock(second_window);
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

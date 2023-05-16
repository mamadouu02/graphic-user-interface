#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget_configure.h"
#include "ei_frame.h"


/*
 * ei_main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_widget_t	frame;
	ei_widget_t	frame2;
	ei_widget_t 	second_frame;
	ei_surface_t 	surface_image;

	/* Create the application and change the color of the background. */
	ei_app_create((ei_size_t){600, 600}, false);
	ei_frame_set_bg_color(ei_app_root_widget(), (ei_color_t){0x52, 0x7f, 0xb4, 0xff});

	/* Create, configure and place the frame on screen. */

	surface_image = hw_image_load("misc/tchoupi.jpg",ei_app_root_surface());
	hw_surface_lock(surface_image);

	ei_rect_t image_rect = ei_rect(ei_point_zero(), hw_surface_get_size(surface_image));
	image_rect = ei_rect_intersect(ei_rect(ei_point(200,300), ei_size(300,100)), image_rect);
	ei_rect_ptr_t image_rect_ptr = &image_rect;

	frame = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure		(frame, &(ei_size_t){300,200},
			   			&(ei_color_t){0x88, 0x88, 0x88, 0xff},
			 			&(int){6},
					 	&(ei_relief_t){ei_relief_raised}, NULL, NULL, NULL , NULL, &surface_image, &image_rect_ptr, &(ei_anchor_t){ei_anc_south});
	ei_place_xy			(frame, 150, 200);

//	frame2 = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
//	ei_frame_configure		(frame2, &(ei_size_t){300,200},
//					   &(ei_color_t){0x88, 0x00, 0x88, 0xff},
//					   &(int){6},
//					   &(ei_relief_t){ei_relief_raised}, NULL, NULL,NULL , NULL, NULL, NULL, NULL);
//	ei_place_xy			(frame2, 500, 500);
//
//	second_frame = ei_widget_create	("frame", frame, NULL, NULL);
//	ei_frame_configure		(second_frame, &(ei_size_t){100,100},
//					   &(ei_color_t){0x00, 0x00, 0xff, 0xff},
//					   &(int){6},
//					   &(ei_relief_t){ei_relief_raised}, &(ei_string_t) {"Ratio mon reuf"}, NULL,&(ei_color_t){0x00, 0xff, 0x00, 0xff} , NULL, NULL, NULL, NULL);
//	ei_place_xy			(second_frame, 150, 200);
	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}

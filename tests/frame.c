#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget_configure.h"


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

	/* Create the application and change the color of the background. */
	ei_app_create((ei_size_t){600, 600}, false);
	ei_frame_set_bg_color(ei_app_root_widget(), (ei_color_t){0x52, 0x7f, 0xb4, 0xff});

	/* Create, configure and place the frame on screen. */
	frame = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure		(frame, &(ei_size_t){300,200},
			   			&(ei_color_t){0x88, 0x88, 0x88, 0xff},
			 			&(int){6},
					 	&(ei_relief_t){ei_relief_raised}, &(ei_string_t) {"RATIO"}, NULL, NULL , NULL, NULL, NULL, NULL);
	ei_place_xy			(frame, 150, 200);

	frame2 = ei_widget_create	("frame", ei_app_root_widget(), NULL, NULL);
	ei_frame_configure		(frame2, &(ei_size_t){300,200},
					   &(ei_color_t){0x88, 0x00, 0x88, 0xff},
					   &(int){6},
					   &(ei_relief_t){ei_relief_raised}, NULL, NULL,NULL , NULL, NULL, NULL, NULL);
	ei_place_xy			(frame2, 500, 500);

	second_frame = ei_widget_create	("frame", frame, NULL, NULL);
	ei_frame_configure		(second_frame, &(ei_size_t){100,100},
					   &(ei_color_t){0x00, 0x00, 0xff, 0xff},
					   &(int){6},
					   &(ei_relief_t){ei_relief_raised}, &(ei_string_t) {"Ratio mon reuf"}, NULL,&(ei_color_t){0x00, 0xff, 0x00, 0xff} , NULL, NULL, NULL, NULL);
	ei_place_xy			(second_frame, 150, 200);
	/* Run the application's main loop. */
	ei_app_run();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}

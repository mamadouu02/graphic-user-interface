/**
 *  @file	ei_application.c
 *  @brief	Manages the main steps of a graphical application: initialization, main window,
 *		main loop, quitting, resource freeing.
 *
 */

#include "ei_application.h"
#include "ei_types.h"
#include "ei_widgetclass.h"
#include "ei_classes.h"



void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
	hw_init();

	ei_widgetclass_t frame;
	frame.allocfunc = &frame_allocfunction;
	frame.releasefunc = &frame_releasefunc;
	frame.drawfunc = &frame_drawfunc;
	frame.setdefaultsfunc = &frame_setdefaultsfunc;

	ei_widgetclass_register(&frame);

	ei_surface_t main_window = hw_create_window(main_window_size, fullscreen);
	ei_surface_t offscreen = hw_surface_create(main_window, main_window_size, false);
	hw_surface_free(main_window);
	hw_surface_free(offscreen);
}

void ei_app_free(void)
{
    /* A implémenter */
}

void ei_app_run(void)
{
	int c;

	do {
		c = getchar();
	} while (c != '\n');

	ei_app_quit_request();
}

void ei_app_invalidate_rect(const ei_rect_t* rect)
{
    /* A implémenter */
}

void ei_app_quit_request(void)
{
    /* A implémenter */
}

ei_widget_t ei_app_root_widget(void)
{
    /* A implémenter */
    return 0;
}

ei_surface_t ei_app_root_surface(void)
{
    /* A implémenter */
    return 0;
}

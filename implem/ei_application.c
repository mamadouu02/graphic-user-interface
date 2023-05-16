/**
 *  @file	ei_application.c
 *  @brief	Manages the main steps of a graphical application: initialization, main window,
 *		main loop, quitting, resource freeing.
 *
 */

#include "ei_application.h"
#include "ei_frame.h"
#include "ei_event.h"

ei_widget_t root;
ei_surface_t main_window, offscreen;

void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
	hw_init();

	ei_frame_register();

	main_window = hw_create_window(main_window_size, fullscreen);
	offscreen = hw_create_window(main_window_size, fullscreen);

	ei_widgetclass_t *frame = ei_widgetclass_from_name("frame");
	root = frame->allocfunc();
	root->wclass = frame;
	ei_widget_set_pick(root);
	root->requested_size = hw_surface_get_size(ei_app_root_surface());
	root->screen_location = ei_rect(ei_point_zero(), root->requested_size);
	root->content_rect = &root->screen_location;
	frame_setdefaultsfunc(root);
}

void ei_app_free(void)
{
	ei_widget_destroy(root);
	hw_surface_free(main_window);
	hw_surface_free(offscreen);
}

void ei_app_run(void)
{
	hw_surface_lock(main_window);
	hw_surface_lock(offscreen);

	ei_widget_t widget = root;
	ei_impl_app_run_children(widget);

	root->wclass->drawfunc(root, main_window, offscreen, NULL);

	hw_surface_unlock(main_window);
	hw_surface_unlock(offscreen);
	hw_surface_update_rects(main_window, NULL);
	hw_surface_update_rects(offscreen, NULL);

	ei_event_t event;
	while ((event.type != ei_ev_close) && (event.type != ei_ev_keydown))
		hw_event_wait_next(&event);
}

void ei_app_invalidate_rect(const ei_rect_t* rect)
{
    /* A implémenter */
}

void ei_app_quit_request(void)
{
	hw_quit();
}

ei_widget_t ei_app_root_widget(void)
{
    	return root;
}

ei_surface_t ei_app_root_surface(void)
{
	return main_window;
}

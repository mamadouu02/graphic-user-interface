/**
 *  @file	ei_application.c
 *  @brief	Manages the main steps of a graphical application: initialization, main window,
 *		main loop, quitting, resource freeing.
 *
 */

#include "ei_application.h"
#include "ei_class.h"
#include "ei_event.h"

ei_widget_t root;
ei_surface_t main_window, offscreen;

void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
	hw_init();

	ei_frame_register();

	main_window = hw_create_window(main_window_size, fullscreen);

	ei_widgetclass_t *frame = ei_widgetclass_from_name("frame");
	root = frame->allocfunc();
	root->wclass = frame;
	root->requested_size = hw_surface_get_size(ei_app_root_surface());
	root->screen_location = ei_rect(ei_point_zero(), root->requested_size);
	root->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
	root->placer_params->rectangle = ei_rect(ei_point_zero(), root->requested_size);
	root->content_rect = &root->screen_location;
	frame_setdefaultsfunc(root);
}

void ei_app_free(void)
{
    hw_surface_free(main_window);
    hw_quit();
}

void ei_app_run(void)
{
	hw_surface_lock(main_window);

	ei_widget_t widget = root->children_head;

	while (widget != NULL) {
		ei_place_calculate(widget);
		widget = widget->next_sibling;
	}

	root->wclass->drawfunc(root, main_window, offscreen, NULL);
	
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

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
    /* A implémenter */
}

ei_widget_t ei_app_root_widget(void)
{
    	return root;
}

ei_surface_t ei_app_root_surface(void)
{
	return main_window;
}

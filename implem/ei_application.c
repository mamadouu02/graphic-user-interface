/**
 *  @file	ei_application.c
 *  @brief	Manages the main steps of a graphical application: initialization, main window,
 *		main loop, quitting, resource freeing.
 *
 */

#include "ei_application.h"
#include "ei_class.h"
#include "ei_event.h"


void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
	hw_init();

	// ei_widgetclass_t *frame = ei_widgetclass_from_name("frame");
	// ei_widgetclass_register(frame);

	ei_surface_t main_window = hw_create_window(main_window_size, fullscreen);
	hw_surface_update_rects(main_window, NULL);
}

void ei_app_free(void)
{
    /* A implémenter */
}

void ei_app_run(void)
{
	// ei_widget_t child = ei_app_root_widget();

	// while (child != NULL){
	// 	ei_widget_t sibling = child;
	// 	child->wclass->drawfunc(child, ei_app_root_surface(), ei_app_root_surface(), NULL);

	// 	while (sibling != NULL){
	// 		sibling->wclass->drawfunc(sibling, ei_app_root_surface(), ei_app_root_surface(), NULL);
	// 		sibling = sibling->next_sibling;
	// 	}
	// 	child = child->children_head;
	// }

	ei_event_t event;
	hw_event_wait_next(&event);
	getchar();
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
	// ei_widget_t root = frame_allocfunction();

	// root->wclass = ei_widgetclass_from_name("frame");
	// root->pick_id = 0;
	// root->pick_color = (ei_color_t *) &ei_default_background_color;
	// root->user_data = NULL;
	// root->destructor = NULL;

	// root->parent = NULL;
	// root->children_head = NULL;
	// root->children_tail = NULL;
	// root->next_sibling = NULL;

	// root->placer_params = NULL;
	// root->requested_size = hw_surface_get_size(ei_app_root_surface());
	// root->requested_size = ei_size(600, 600);
	// root->screen_location = ei_rect(ei_point_zero(), root->requested_size);
	// root->content_rect = &root->screen_location;

	// frame_setdefaultsfunc(root);

    	// return root;
	return 0;
}

ei_surface_t ei_app_root_surface(void)
{
	// ei_surface_t surface = hw_surface_create();
    	// return surface;
	return 0;
}

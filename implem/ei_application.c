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
ei_surface_t main_window = NULL;
ei_surface_t offscreen = NULL;


void ei_app_create(ei_size_t main_window_size, bool fullscreen)
{
	hw_init();

	//ei_widgetclass_t *frame = ei_widgetclass_from_name("frame");
	//ei_widgetclass_register(frame);

	main_window = hw_create_window(main_window_size, fullscreen);

	root = frame_allocfunction();

	root->wclass = ei_widgetclass_from_name("frame");
	root->pick_id = 0;
	root->pick_color = NULL;
	root->user_data = NULL;
	root->destructor = NULL;

	root->parent = NULL;
	root->children_head = NULL;
	root->children_tail = NULL;
	root->next_sibling = NULL;

	root->placer_params = NULL;
	root->requested_size = hw_surface_get_size(ei_app_root_surface());
	root->screen_location = ei_rect(ei_point_zero(), root->requested_size);
	root->content_rect = &root->screen_location;

	frame_setdefaultsfunc(root);

//	free(frame);
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
	hw_surface_unlock(main_window);

	frame_drawfunc(root, main_window, offscreen, NULL);
	hw_surface_update_rects(main_window, NULL);

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
    	return root;
}

ei_surface_t ei_app_root_surface(void)
{
	return main_window;
}

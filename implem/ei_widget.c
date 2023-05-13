/**
 * @file	ei_widget.c
 *
 * @brief 	API for widgets management: creation, destruction
 *
 */

#include "ei_widget.h"
#include "ei_class.h"

ei_widget_t ei_widget_create(ei_const_string_t class_name, ei_widget_t parent, ei_user_param_t user_data, ei_widget_destructor_t destructor)
{
	ei_widget_t widget = frame_allocfunction();

	widget->wclass = ei_widgetclass_from_name(class_name);
	widget->wclass->next = NULL;
	widget->pick_id = 0;
	widget->pick_color = NULL;
	widget->user_data = user_data;
	widget->destructor = destructor;

	/* Widget Hierachy Management */
	widget->parent = parent;
	widget->children_head = NULL;
	widget->children_tail = NULL;
	widget->next_sibling = NULL;

	/* Geometry Management */
	widget->placer_params = NULL;
	widget->requested_size = ei_size(0, 0);
	widget->screen_location = ei_rect(ei_point(0, 0), ei_size(0, 0));
	widget->content_rect = &widget->screen_location;

	return widget;
}

void ei_widget_destroy (ei_widget_t widget)
{
        ei_widget_t child = widget;
		ei_widget_t next_sibling = widget->next_sibling;
		while (child != NULL) {
			while (next_sibling != NULL) {
				next_sibling = next_sibling->next_sibling;
			}
			child = child->children_head;
			next_sibling = child;
		}

}

bool ei_widget_is_displayed (ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

ei_widget_t ei_widget_pick (ei_point_t* where)
{
        /* A implémenter */
        return 0;
}

/**
 * @file	ei_widget.c
 *
 * @brief 	API for widgets management: creation, destruction
 *
 */

#include "ei_widget.h"
#include "ei_frame.h"
#include "ei_application.h"

extern ei_surface_t offscreen;

ei_widget_t ei_widget_create(ei_const_string_t class_name, ei_widget_t parent, ei_user_param_t user_data, ei_widget_destructor_t destructor)
{
	ei_widgetclass_t *class = ei_widgetclass_from_name(class_name);
	ei_widget_t widget = class->allocfunc();

	widget->wclass = class;
	ei_widget_set_pick(widget);
	widget->user_data = user_data;
	widget->destructor = destructor;
	widget->parent = parent;

	if (parent->children_tail) {
		parent->children_tail->next_sibling = widget;
		parent->children_tail = widget;
	} else {
		parent->children_head = widget;
		parent->children_tail = widget;
	}

	if (!strcmp(widget->wclass->name,"toplevel")) {
		widget->requested_size = ei_size(320, 240);
	} else {
		widget->requested_size = ei_size(widget->parent->content_rect->size.width/20, widget->parent->content_rect->size.height/20);
	}

	widget->screen_location = *widget->parent->content_rect;
	widget->content_rect = &(widget->screen_location);
	class->setdefaultsfunc(widget);

	return widget;
}

void ei_widget_destroy(ei_widget_t widget)
{
	if (widget != NULL) {
		ei_widget_t child = widget->children_head;
		ei_widget_t next_child;

		if (child != NULL) {
			next_child = child->next_sibling;
		}

		while (child != NULL) {
			if (child->children_head != NULL) {
				ei_widget_destroy(child);
			}

			//child->wclass->releasefunc(child);
			free(child);

			child = next_child;
			next_child = (child == NULL) ? NULL : child->next_sibling;
		}
	}
}

bool ei_widget_is_displayed(ei_widget_t widget)
{
        uint32_t pick_id = widget->pick_id;
        ei_point_t top_left = widget->screen_location.top_left;
        ei_widget_t widget_top_left = ei_widget_pick(&top_left);

	return widget_top_left->pick_id == pick_id;
}

ei_widget_t ei_widget_pick(ei_point_t* where)
{
	hw_surface_lock(offscreen);

	uint8_t *pix = hw_surface_get_buffer(offscreen);
	ei_size_t size = hw_surface_get_size(offscreen);
	int ir, ig, ib, ia;
	hw_surface_get_channel_indices(offscreen, &ir, &ig, &ib, &ia);
	uint32_t pick_id = pix[4 * (where->y * size.width + where->x) + ir];
	ei_widget_t *widget_ptr = malloc(sizeof(ei_widget_t));
	ei_pick(ei_app_root_widget(), pick_id, widget_ptr);

	hw_surface_unlock(offscreen);

	return *widget_ptr;
}

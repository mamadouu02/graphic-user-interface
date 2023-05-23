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

ei_widget_t ei_widget_create(ei_const_string_t class_name, ei_widget_t parent, ei_user_param_t user_data, ei_widget_destructor_t destructor) {
	ei_widgetclass_t *class = ei_widgetclass_from_name(class_name);
	ei_widget_t widget = class->allocfunc();

	widget->wclass = class;
	ei_widget_set_pick(widget);

	if (user_data) {
		widget->user_data = user_data;
	}

	widget->destructor = destructor;
	widget->parent = parent;

	if (parent->children_tail) {
		parent->children_tail->next_sibling = widget;
		parent->children_tail = widget;
	} else {
		parent->children_head = widget;
		parent->children_tail = widget;
	}

	widget->screen_location = *widget->parent->content_rect;
	widget->content_rect = &widget->screen_location;

	if (!strcmp(widget->wclass->name, "toplevel")) {
		widget->requested_size = ei_size(320, 240);

		ei_widget_t resize_frame = ei_widget_create("frame", widget, NULL, NULL);
		resize_frame->wclass->setdefaultsfunc(resize_frame);
		resize_frame->placer_params = calloc(1, sizeof(struct ei_impl_placer_params_t));
		((ei_impl_frame_t *) resize_frame)->color = (ei_color_t) { 0.7 * 0xA0, 0.7 * 0xA0, 0.7 * 0xA0, 0xFF };

		ei_widget_t close_button = ei_widget_create("button", widget, NULL, NULL);
		close_button->wclass->setdefaultsfunc(close_button);
		close_button->placer_params = calloc(1, sizeof(struct ei_impl_placer_params_t));
		((ei_impl_button_t *) close_button)->color = (ei_color_t) { 0xFF, 0x00, 0x00, 0xA0 };
	} else {
		ei_size_t size = widget->parent->content_rect->size;
		widget->requested_size = ei_size(size.width / 20, size.height / 20);
	}

	widget->wclass->setdefaultsfunc(widget);

	return widget;
}

void ei_widget_destroy(ei_widget_t widget)
{
	if (widget->parent) {
		ei_widget_t parent = widget->parent;
		ei_widget_t prev_child = NULL;
		ei_widget_t child = parent->children_head;

		while (child != widget) {
			prev_child = child;
			child = child->next_sibling;
		}

		if (prev_child == NULL) {
			parent->children_head = widget->next_sibling;

			if (child->next_sibling == NULL) {
				parent->children_tail = NULL;
			}
		} else {
			prev_child->next_sibling = widget->next_sibling;

			if (widget->next_sibling == NULL) {
				parent->children_tail = prev_child;
			}
		}
	}

	widget->next_sibling = NULL;

	ei_widget_destroy_children(widget);
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

	uint8_t *pix_ptr = hw_surface_get_buffer(offscreen);
	ei_size_t size = hw_surface_get_size(offscreen);
	int ir, ig, ib, ia;
	hw_surface_get_channel_indices(offscreen, &ir, &ig, &ib, &ia);
	uint32_t pick_id = pix_ptr[4 * (where->y * size.width + where->x) + ir];
	ei_widget_t *widget_ptr = malloc(sizeof(ei_widget_t));
	ei_pick(ei_app_root_widget(), pick_id, widget_ptr);

	hw_surface_unlock(offscreen);

	ei_widget_t widget = *widget_ptr;
	free(widget_ptr);

	return widget;
}

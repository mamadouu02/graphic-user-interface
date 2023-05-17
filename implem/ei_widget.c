/**
 * @file	ei_widget.c
 *
 * @brief 	API for widgets management: creation, destruction
 *
 */

#include "ei_widget.h"
#include "ei_implementation.h"
#include "ei_frame.h"

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

			//child->wclass->releasefunc(child); /* * Libération d'adresses non allouées */
			free(child);

			child = next_child;
			next_child = child == NULL ? NULL : child->next_sibling;
		}
	}

}

bool ei_widget_is_displayed(ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

ei_widget_t ei_widget_pick(ei_point_t* where)
{
        /* A implémenter */
        return 0;
}

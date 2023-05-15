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
	ei_widgetclass_t *class = ei_widgetclass_from_name(class_name);
	ei_widget_t widget = class->allocfunc();

	widget->wclass = class;
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

	widget->content_rect = &(widget->parent->screen_location);
	class->setdefaultsfunc(widget);

	return widget;
}

void ei_widget_destroy(ei_widget_t widget)
{
        // ei_widget_t child = widget;
	// 	ei_widget_t next_sibling = widget->next_sibling;
	// 	while (child != NULL) {
	// 		while (next_sibling != NULL) {
	// 			next_sibling = next_sibling->next_sibling;
	// 		}
	// 		child = child->children_head;
	// 		next_sibling = child;
	// 	}
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

/**
 *  @file	ei_placer.c
 *  @brief	Manages the positioning and sizing of widgets on the screen.
 *
 */

#include "ei_placer.h"
#include "ei_implementation.h"


void	ei_place	(ei_widget_t		widget,
			ei_anchor_t*		anchor,
			int*			x,
			int*			y,
			int*			width,
			int*			height,
			float*			rel_x,
			float*			rel_y,
			float*			rel_width,
			float*			rel_height)
{
	int parent_height = widget->parent->content_rect->size.height;
	int parent_width = widget->parent->content_rect->size.width;

	int height_rel = (rel_height == NULL) ? -1 : *rel_height * parent_height;
	int width_rel = (rel_width == NULL) ? -1 : *rel_width * parent_width;
	int *height_place = (height_rel == -1) ? ((height == NULL) ? NULL : height) : &height_rel;
	int *width_place = (width_rel == -1) ? ((width == NULL) ? NULL : width) : &width_rel;
	int height_widget = (height_place == NULL) ? widget->requested_size.height : *height_place;
	int width_widget = (width_place == NULL) ? widget->requested_size.width : *width_place;

	ei_point_t *where = malloc(sizeof(ei_point_t));
	where->x = ((rel_x == NULL) ? 0 : parent_width * (*rel_x)) + (x == NULL ? 0 : *x);
	where->y = ((rel_y == NULL) ? 0 : parent_height * (*rel_y)) + (y == NULL ? 0 : *y);

	ei_rect_t widget_rectangle = ei_rect(*where, ei_size(width_widget, height_widget));
	ei_anchor_t widget_anchor = (anchor == NULL) ? ei_anc_northwest : *anchor;

	if (widget->placer_params == NULL) {
		widget->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
	}

	widget->placer_params->anchor = &widget_anchor;
	widget->placer_params->rectangle = &widget_rectangle;

	free(where);
}

void ei_placer_forget(ei_widget_t widget)
{
	/* A implémenter */
}

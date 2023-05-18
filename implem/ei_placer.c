/**
 *  @file	ei_placer.c
 * 
 *  @brief	Manages the positioning and sizing of widgets on the screen.
 *
 */

#include "ei_placer.h"
#include "ei_implementation.h"
#include "ei_application.h"

extern ei_surface_t offscreen;

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
	if (widget->placer_params == NULL) {
		widget->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
	}

	widget->placer_params->x = (x == NULL) ? 0 : *x;
	widget->placer_params->y = (y == NULL) ? 0 : *y;
	widget->placer_params->rel_x = (rel_x == NULL) ? 0.0 : *rel_x;
	widget->placer_params->rel_y = (rel_y == NULL) ? 0.0 : *rel_y;
	widget->placer_params->width = (rel_width == NULL) ? ((width == NULL) ?  widget->requested_size.width : *width) : 0;
	widget->placer_params->height = (rel_height == NULL) ? ((height == NULL) ?  widget->requested_size.height : *height) : 0;
	widget->placer_params->rel_width = (rel_width == NULL) ? NULL : rel_width;
	widget->placer_params->rel_height = (rel_height == NULL) ? NULL : rel_height;
	widget->placer_params->anchor = (anchor == NULL) ? ei_anc_northwest : *anchor;
}

void ei_placer_forget(ei_widget_t widget)
{
	widget->placer_params = NULL;
	ei_surface_t surface = ei_app_root_surface();

	widget->parent->wclass->drawfunc(widget->parent, surface, offscreen, NULL);
}

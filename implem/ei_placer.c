/**
 *  @file	ei_placer.c
 * 
 *  @brief	Manages the positioning and sizing of widgets on the screen.
 *
 */

#include "ei_placer.h"
#include "ei_implementation.h"
#include "ei_application.h"
#include "ei_frame.h"

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

	widget->placer_params->anchor = (anchor) ? *anchor : ei_anc_northwest;
	widget->placer_params->x = (x) ? *x : 0;
	widget->placer_params->y = (y) ? *y : 0;
	widget->placer_params->width = (rel_width) ? 0 : ((width) ? *width : widget->requested_size.width);
	widget->placer_params->height = (rel_height) ? 0 : ((height) ? *height : widget->requested_size.height);
	widget->placer_params->rel_x = (rel_x) ? *rel_x : 0.0;
	widget->placer_params->rel_y = (rel_y) ? *rel_y : 0.0;
	widget->placer_params->rel_width = (rel_width) ? rel_width : NULL;
	widget->placer_params->rel_height = (rel_height) ? rel_height : NULL;

	if (!strcmp(widget->wclass->name, "toplevel")) {
		int hauteur = 25;
		widget->placer_params->y += hauteur;
		widget->placer_params->height += 25;
	}

	if (widget->parent && !strcmp(widget->parent->wclass->name, "toplevel")) {
		if (widget->placer_params->rel_y == 0) {
			widget->placer_params->y += 25;
		}
	}
}

void ei_placer_forget(ei_widget_t widget)
{
	widget->placer_params = NULL;
	widget->parent->wclass->drawfunc(widget->parent, ei_app_root_surface(), offscreen, NULL);
}

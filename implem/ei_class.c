/**
 * @file	ei_classes.c
 *
 * @brief 	Classes definitions.
 *
 */

#include "ei_class.h"

ei_widget_t frame_allocfunction(void)
{
	ei_widget_t widget = calloc(ei_widget_struct_size(), sizeof(ei_impl_frame_t));
	return widget;
}

void frame_releasefunc(ei_widget_t widget)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	free(frame->requested_size);
	free((void *) frame->color);
	free(frame->border_width);
	free(frame->relief);
	free(frame->text);
	free(frame->text_font);
	free(frame->text_color);
	free(frame->text_anchor);
	free(frame->img);
	free(frame->img_rect);
	free(frame->img_anchor);
}

void frame_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_widget_t child = widget;

	while (child != NULL) {
		ei_impl_frame_t* frame = (ei_impl_frame_t *) widget;

		ei_rect_t final_location = rect_intersection(widget->screen_location, *clipper);
		ei_draw_text(surface, &final_location.top_left, (ei_const_string_t) frame->text, frame->text_font, *(frame->text_color), clipper);

		ei_rect_t rect_image = rect_intersection(**(frame->img_rect), *clipper);
		ei_copy_surface(surface, &rect_image, frame->img, *(frame->img_rect), true);

		child = child->children_head;
	}
}

void frame_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	frame->requested_size = NULL;
	frame->color = &ei_default_background_color;
	frame->border_width = NULL;
	frame->relief = NULL;
	frame->text = NULL;
	frame->text_font = &ei_default_font;
	frame->text_color = NULL;
	frame->text_anchor = NULL;
	frame->img = NULL;
	frame->img_rect = NULL;
	frame->img_anchor = NULL;
}

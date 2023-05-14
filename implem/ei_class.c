/**
 * @file	ei_classes.c
 *
 * @brief 	Classes definitions.
 *
 */

#include "ei_class.h"

ei_widget_t frame_allocfunction(void)
{
	ei_widget_t frame = calloc(1, sizeof(ei_impl_frame_t));
	return frame;
}

void frame_releasefunc(ei_widget_t widget)
{
	if (widget != NULL) {
		ei_widget_t child = widget->children_head;
		ei_widget_t next_child;

		if (child != NULL) {
			next_child = child->next_sibling;
		}

		while (child != NULL) {
			if (child->children_head != NULL) {
				frame_releasefunc(child);
			}

			ei_impl_frame_t *frame = (ei_impl_frame_t *) child;
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
			free(frame);
			free(child);

			child = next_child;
			next_child = child == NULL ? NULL : child->next_sibling;
		}
	}
}

void frame_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_widget_t child = widget;

	while (child != NULL) {
		ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;

//		ei_rect_t final_location;
//		ei_rect_t rect_image;


//		if (clipper == NULL) {
//			final_location = widget->screen_location;
//			rect_image = **(frame->img_rect);
//		} else
//			final_location = rect_intersection(widget->screen_location, *clipper);
//			rect_image = rect_intersection(**(frame->img_rect), *clipper);
//		}

		ei_fill(surface, frame->color, clipper);

//		ei_draw_text(surface, &final_location.top_left, (ei_const_string_t) frame->text, frame->text_font, *(frame->text_color), clipper);

//		ei_copy_surface(surface, &rect_image, frame->img, *(frame->img_rect), true);

		child = child->children_head;
	}
}


void frame_setdefaultsfunc(ei_widget_t widget)
{
	ei_relief_t relief = ei_relief_none;
	ei_color_t font_color = ei_font_default_color;
	ei_anchor_t anchor = ei_anc_center;

	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	frame->color = &ei_default_background_color;
	frame->relief = &relief;
	frame->text_font = &ei_default_font;
	frame->text_color = &font_color;
	frame->text_anchor = &anchor;
	frame->img_anchor = &anchor;
}

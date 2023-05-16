/**
 * @file	ei_frame.c
 *
 * @brief 	Frame class.
 *
 */

#include "ei_frame.h"
#include "ei_polygon.h"


ei_widget_t frame_allocfunction(void)
{
	return calloc(1, sizeof(ei_impl_frame_t));
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
			free(frame);
			free(child);

			child = next_child;
			next_child = child == NULL ? NULL : child->next_sibling;
		}
	}
}

void frame_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &frame->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
	} else if (widget->placer_params) {

		ei_rect_t widget_rect;
		widget_rect = widget->screen_location;

		ei_rect_t clipper_frame = *widget->parent->content_rect;
		if (clipper) {
			clipper_frame = ei_rect_intersect(clipper_frame, *clipper);
		}

		ei_draw_frame(surface, widget_rect, frame->color, frame->relief, &clipper_frame);
		ei_draw_frame(pick_surface, widget_rect, widget->pick_color, ei_relief_none, &clipper_frame);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, clipper_frame);
		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;
		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;
		if (child) {
			child->screen_location = ei_rect_intersect(*widget->content_rect,widget->children_head->screen_location);
			child->content_rect = &widget->children_head->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t clipper_text_image = new_screen_loc;
		if (clipper) {
			clipper_text_image = ei_rect_intersect(clipper_text_image, *clipper);
		}

		if (frame->text) {
			ei_surface_t surface_text = hw_text_create_surface(frame->text, frame->text_font,
									   frame->text_color);
			ei_rect_t text_rect = hw_surface_get_rect(surface_text);
			text_rect.top_left = ei_anchor_text_img(&(frame->text_anchor), &text_rect, &clipper_text_image);

			ei_draw_text(surface, &text_rect.top_left, (ei_const_string_t) frame->text, frame->text_font,
				     frame->text_color, &clipper_text_image);
		} else if (frame->img) {
			ei_rect_t img_rect = (clipper) ? ei_rect_intersect(*(frame->img_rect), *clipper) : *(frame->img_rect);
			img_rect.top_left = clipper_text_image.top_left;
			img_rect = ei_rect_intersect(img_rect,clipper_text_image);
			img_rect.top_left = ei_anchor_text_img(&frame->img_anchor, &img_rect, &clipper_text_image);

			ei_rect_cpy(surface, &img_rect, frame->img, frame->img_rect, true);
		}
	}

	ei_impl_widget_draw_children(widget->children_head, surface, pick_surface, clipper);
}

void frame_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	frame->color = ei_default_background_color;
	frame->relief = (ei_relief_t) { ei_relief_none };
	frame->text_font = ei_default_font;
	frame->text_color = ei_font_default_color;
	frame->text_anchor = (ei_anchor_t) { ei_anc_center };
	frame->img_anchor = (ei_anchor_t) { ei_anc_center };
}

void ei_frame_register(void)
{
	ei_widgetclass_t *frame = calloc(1, sizeof(ei_widgetclass_t));
	strcpy(frame->name, "frame");
	frame->allocfunc = &frame_allocfunction;
	frame->releasefunc = &frame_releasefunc;
	frame->drawfunc = &frame_drawfunc;
	frame->setdefaultsfunc = &frame_setdefaultsfunc;
	ei_widgetclass_register(frame);
}

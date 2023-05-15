/**
 * @file	ei_class.c
 *
 * @brief 	Classes definitions.
 *
 */

#include "ei_class.h"

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
	} else if (widget->placer_params != NULL) {

		ei_rect_t widget_rect;
		widget_rect = widget->placer_params->rectangle;
		widget_rect.top_left = anchor_rect(&widget->placer_params->anchor, &widget_rect);

		ei_rect_t clipper_frame = *widget->content_rect;
		if (clipper != NULL) {
			clipper_frame = rect_intersection(clipper_frame, *clipper);
		}
		draw_frame(surface, widget_rect, frame->color, frame->relief, &clipper_frame);


		ei_rect_t clipper_text_image = widget_rect;
		if (clipper != NULL) {
			clipper_text_image = rect_intersection(clipper_text_image, *clipper);
		}

		if (frame->text) {
			ei_surface_t surface_text = hw_text_create_surface(frame->text, frame->text_font, frame->text_color);
			ei_rect_t text_rect = hw_surface_get_rect(surface_text);

			text_rect.top_left = anchor_text_image(&(frame->text_anchor), &text_rect, &clipper_text_image);
			if (!rect_cmp(*widget->content_rect, clipper_frame)) {
				text_rect.top_left.x += clipper_frame.top_left.x;
				text_rect.top_left.y += clipper_frame.top_left.y;
			} else{
				text_rect.top_left.x += widget_rect.top_left.x;
				text_rect.top_left.y += widget_rect.top_left.y;
			}
			ei_draw_text(surface, &text_rect.top_left, (ei_const_string_t) frame->text, frame->text_font,
				frame->text_color, clipper);
		} else if (frame->img) {
			ei_rect_t img_rect = (clipper) ? rect_intersection(*(frame->img_rect), *clipper)
						       : *(frame->img_rect);
			img_rect.top_left = anchor_text_image(&frame->img_anchor, &img_rect, &clipper_text_image);
			img_rect.top_left.x+=widget_rect.top_left.x;
			img_rect.top_left.y+=widget_rect.top_left.y;
			ei_copy_surface(surface, &img_rect, frame->img, frame->img_rect, true);
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

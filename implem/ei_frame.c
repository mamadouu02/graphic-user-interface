/**
 * @file	ei_frame.c
 *
 * @brief 	Frame class.
 *
 */

#include "ei_frame.h"

extern ei_surface_t offscreen;
bool resize = false;

ei_widget_t frame_allocfunction(void)
{
	return calloc(1, sizeof(ei_impl_frame_t));
}

void frame_releasefunc(ei_widget_t widget)
{
	free(widget->user_data);
	free(widget->content_rect);
	free(widget->placer_params);

	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	free(frame->text);
	free(frame->text_font);
	free(frame->img);
	free(frame->img_rect);
}

void frame_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &frame->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
		ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
	} else if (widget->placer_params) {
		ei_rect_t widget_rect = widget->screen_location;
		ei_rect_t frame_clipper = *widget->parent->content_rect;
		
		if (clipper) {
			frame_clipper = ei_rect_intersect(frame_clipper, *clipper);
		}

		ei_draw_frame(surface, widget_rect, frame->color, frame->relief, &frame_clipper);
		ei_draw_frame(pick_surface, widget_rect, widget->pick_color, ei_relief_none, &frame_clipper);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, frame_clipper);
		
		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;
		
		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;
		
		while (child) {
			child->screen_location = ei_rect_intersect(*widget->content_rect, child->screen_location);
			child->content_rect = &child->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t text_img_clipper = new_screen_loc;
		
		if (clipper) {
			text_img_clipper = ei_rect_intersect(text_img_clipper, *clipper);
		}

		if (frame->text) {
			ei_surface_t text_surface = hw_text_create_surface(frame->text, frame->text_font, frame->text_color);
			ei_rect_t text_rect = hw_surface_get_rect(text_surface);
			text_rect.top_left = ei_anchor_text_img(&(frame->text_anchor), &text_rect, &text_img_clipper);
			ei_draw_text(surface, &text_rect.top_left, (ei_const_string_t) frame->text, frame->text_font, frame->text_color, &text_img_clipper);
			hw_surface_free(text_surface);
		} else if (frame->img) {
			ei_rect_t img_rect = (clipper) ? ei_rect_intersect(*(frame->img_rect), *clipper) : *(frame->img_rect);
			img_rect.top_left = text_img_clipper.top_left;
			img_rect = ei_rect_intersect(img_rect, text_img_clipper);
			img_rect.top_left = ei_anchor_text_img(&frame->img_anchor, &img_rect, &text_img_clipper);
			ei_rect_cpy(surface, &img_rect, frame->img, frame->img_rect, true);
		}

		ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
	}
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

bool frame_handlefunc(ei_widget_t widget, struct ei_event_t* event)
{
	if (widget->parent != NULL && !strcmp(widget->parent->wclass->name, "toplevel") && widget == widget->parent->children_head) {
		hw_surface_lock(ei_app_root_surface());
		hw_surface_lock(offscreen);

		ei_rect_t toplevel_rect = widget->screen_location;

		switch (event->type) {
			case ei_ev_mouse_buttondown:
				if (event->param.mouse.button == ei_mouse_button_left && ei_in_rect(event->param.mouse.where, toplevel_rect)) {
					ei_event_set_active_widget(widget);
				}
				break;
			case ei_ev_mouse_buttonup:
				ei_event_set_active_widget(NULL);
				break;
			case ei_ev_mouse_move:
				if (ei_event_get_active_widget() == widget) {
					int dx = event->param.mouse.where.x - ((ei_point_t *) widget->my_param)->x;
					int dy = event->param.mouse.where.y - ((ei_point_t *) widget->my_param)->y;

					if (resize) {
						ei_placer_forget(widget->parent);
						ei_toplevel_resize_update(widget->parent, dx, dy);

						hw_surface_unlock(ei_app_root_surface());
						hw_surface_unlock(offscreen);

						hw_surface_update_rects(ei_app_root_surface(), NULL);

						hw_surface_lock(ei_app_root_surface());
						hw_surface_lock(offscreen);

						widget->parent->wclass->drawfunc(widget->parent, ei_app_root_surface(), offscreen, NULL);
					}

					resize = !resize;
				}
				break;
			default:
				break;
		}

		hw_surface_unlock(ei_app_root_surface());
		hw_surface_unlock(offscreen);

		hw_surface_update_rects(ei_app_root_surface(), NULL);
	}

	return true;
}

void ei_frame_register(void)
{
	ei_widgetclass_t *frame = calloc(1, sizeof(ei_widgetclass_t));
	strcpy(frame->name, "frame");
	frame->allocfunc = &frame_allocfunction;
	frame->releasefunc = &frame_releasefunc;
	frame->drawfunc = &frame_drawfunc;
	frame->setdefaultsfunc = &frame_setdefaultsfunc;
	frame->handlefunc = &frame_handlefunc;
	ei_widgetclass_register(frame);
}

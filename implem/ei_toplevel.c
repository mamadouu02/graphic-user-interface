/**
 * @file	ei_toplevel.c
 *
 * @brief 	Toplevel class.
 *
 */

#include "ei_toplevel.h"

extern ei_surface_t offscreen;
bool move = false;

ei_widget_t toplevel_allocfunction(void)
{
	return calloc(1, sizeof(ei_impl_toplevel_t));
}

void toplevel_releasefunc(ei_widget_t widget)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;

	if (toplevel->title)
		free(toplevel->title);
}

void toplevel_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &toplevel->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
		ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
	} else if (widget->placer_params) {
		ei_rect_t header_rect = widget->screen_location;
		ei_rect_t toplevel_clipper = *widget->parent->content_rect;

		if (clipper) {
			toplevel_clipper = ei_rect_intersect(toplevel_clipper, *clipper);
		}

		ei_color_t header_color;
		header_color.red = 0.7 * toplevel->color.red;
		header_color.green = 0.7 * toplevel->color.green;
		header_color.blue = 0.7 * toplevel->color.blue;
		header_color.alpha = toplevel->color.alpha;

		ei_draw_frame(surface, header_rect, header_color, ei_relief_none, &toplevel_clipper);
		ei_draw_frame(pick_surface, header_rect, widget->pick_color, ei_relief_none, &toplevel_clipper);

		int header_height = 25;
		int border_width = 3;
		ei_rect_t widget_rect = header_rect;
		widget_rect.top_left.x += border_width;
		widget_rect.top_left.y += header_height;
		widget_rect.size.width -= 2 * border_width;
		widget_rect.size.height -= header_height + border_width;

		ei_draw_frame(surface, widget_rect, toplevel->color, ei_relief_none, &toplevel_clipper);

		/* where to place children */
		widget->content_rect = &widget->screen_location;

		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;

		while (child) {
			if (child == widget->children_head) {
				if (toplevel->resizable != ei_axis_none) {
					((ei_impl_frame_t *) child)->color = header_color;
					int size = 0.05 * widget->screen_location.size.height;

					ei_point_t bottom_right = widget->screen_location.top_left;
					bottom_right.x += widget->screen_location.size.width;
					bottom_right.y += widget->screen_location.size.height;

					child->screen_location.top_left.x = bottom_right.x - size;
					child->screen_location.top_left.y = bottom_right.y - size;
					child->screen_location.size = ei_size(size, size);
				} else {
					child->screen_location = ei_rect_zero();
				}
			} else if (child == widget->children_head->next_sibling) {
				if (toplevel->closable) {
					int size = 17;

					ei_point_t top_right = widget->screen_location.top_left;
					top_right.x += widget->screen_location.size.width;

					child->screen_location.top_left.x = top_right.x - size - border_width;
					child->screen_location.top_left.y = top_right.y + 4;
					child->screen_location.size = ei_size(size, size);
				} else {
					child->screen_location = ei_rect_zero();
				}
			}

			child->content_rect = &child->screen_location;
			child = child->next_sibling;
		}

		header_rect.top_left.x += border_width;
		header_rect.size.height = header_height;
		ei_rect_t text_clipper = ei_rect_intersect(header_rect, toplevel_clipper);

		if (toplevel->title) {
			ei_surface_t text_surface = hw_text_create_surface(toplevel->title, ei_default_font, ei_font_default_color);
			ei_rect_t text_rect = hw_surface_get_rect(text_surface);
			text_rect.top_left = ei_anchor_text_img(&(ei_anchor_t) { ei_anc_west }, &text_rect, &text_clipper);
			ei_rect_t inter = ei_rect_intersect(text_rect, header_rect);
			inter = ei_rect_intersect(inter, *widget->parent->content_rect);
			inter.size.height = text_clipper.size.height;
			inter.size.width = text_clipper.size.width;

			ei_draw_text(surface, &widget->screen_location.top_left, (ei_const_string_t) toplevel->title, ei_default_font, (ei_color_t) { 0xDD, 0xDD, 0xDD, 0xFF }, &inter);
			hw_surface_free(text_surface);
		}

		ei_rect_t new_clipper = ei_rect_intersect(toplevel_clipper, *widget->content_rect);

		ei_impl_widget_draw_children(widget, surface, pick_surface, &new_clipper);
		widget->children_head->wclass->drawfunc(widget->children_head, surface, pick_surface, &new_clipper);
	}
}

void toplevel_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;
	toplevel->color = ei_default_background_color;
	toplevel->border_width = 4;
	toplevel->title = "Toplevel";
	toplevel->closable = true;
	toplevel->resizable = ei_axis_both;
	toplevel->min_size = &(ei_size_t) { 160, 120 };
}

bool ei_toplevel_handlefunc(ei_widget_t widget, struct ei_event_t* event)
{
	hw_surface_lock(ei_app_root_surface());
	hw_surface_lock(offscreen);

	ei_rect_t toplevel_rect = widget->screen_location;
	toplevel_rect.size.height = 25;

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

				if (move) {
					ei_placer_forget(widget);
					ei_toplevel_move_update(widget, dx, dy);
					widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
				}
				
				move = !move;
			}
			break;
		default:
			break;
	}

	hw_surface_unlock(ei_app_root_surface());
	hw_surface_unlock(offscreen);

	hw_surface_update_rects(ei_app_root_surface(), NULL);

	return true;
}

void ei_toplevel_register(void)
{
	ei_widgetclass_t *toplevel = calloc(1, sizeof(ei_widgetclass_t));
	strcpy(toplevel->name, "toplevel");
	toplevel->allocfunc = &toplevel_allocfunction;
	toplevel->releasefunc = &toplevel_releasefunc;
	toplevel->drawfunc = &toplevel_drawfunc;
	toplevel->setdefaultsfunc = &toplevel_setdefaultsfunc;
	toplevel->handlefunc = &ei_toplevel_handlefunc;
	ei_widgetclass_register(toplevel);
}


void ei_toplevel_resize_update(ei_widget_t widget, int dx, int dy)
{
	if (widget) {
		if (strcmp(widget->wclass->name, "toplevel")) {
			ei_impl_placer_run(widget);
		} else {
			widget->screen_location.size.width += dx;
			widget->screen_location.size.height += dy;
			widget->content_rect->size.width += dx;
			widget->content_rect->size.height += dy;
			widget->placer_params = calloc(1, sizeof(struct ei_impl_placer_params_t));
		}

		ei_widget_t child = widget->children_head;

		while (child) {
			ei_toplevel_resize_update(child, dx, dy);
			child = child->next_sibling;
		}
	}
}

void ei_toplevel_move_update(ei_widget_t widget, int dx, int dy)
{
	if (widget) {
		widget->screen_location.top_left.x += dx;
		widget->screen_location.top_left.y += dy;
		widget->content_rect->top_left.x += dx;
		widget->content_rect->top_left.y += dy;

		if (!strcmp(widget->wclass->name, "toplevel")) {
			if (widget->placer_params)
				free(widget->placer_params);

			widget->placer_params = calloc(1, sizeof(struct ei_impl_placer_params_t));
			widget->placer_params->x = widget->screen_location.top_left.x;
			widget->placer_params->y = widget->screen_location.top_left.y;
			widget->placer_params->width = widget->screen_location.size.width;
			widget->placer_params->height = widget->screen_location.size.height;
		}

		ei_widget_t child = widget->children_head;

		while (child) {
			ei_toplevel_move_update(child, dx, dy);
			child = child->next_sibling;
		}
	}
}

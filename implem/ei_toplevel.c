/**
 * @file	ei_toplevel.c
 *
 * @brief 	Toplevel class.
 *
 */

#include "ei_toplevel.h"

extern ei_surface_t offscreen;
bool moving = false;

ei_widget_t toplevel_allocfunction(void)
{
	return calloc(1, sizeof(ei_impl_toplevel_t));
}

void toplevel_releasefunc(ei_widget_t widget)
{
	/* à implémenter */
}

void toplevel_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &toplevel->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
		ei_impl_widget_draw_children(widget->children_head, surface, pick_surface, clipper);
	} else if (widget->placer_params) {
		ei_rect_t widget_rect = widget->screen_location;
		ei_rect_t toplevel_clipper = *widget->parent->content_rect;

		if (clipper) {
			toplevel_clipper = ei_rect_intersect(toplevel_clipper, *clipper);
		}

		ei_draw_frame(surface, widget_rect, toplevel->color, ei_relief_none, &toplevel_clipper);
		ei_draw_frame(pick_surface, widget_rect, widget->pick_color, ei_relief_none, &toplevel_clipper);

		ei_rect_t toplevel_widget_rect = widget_rect;
		int hauteur = 25;
		toplevel_widget_rect.size.height = hauteur;

		ei_color_t toplevel_color;
		toplevel_color.red = 0.7 * toplevel->color.red;
		toplevel_color.green = 0.7 * toplevel->color.green;
		toplevel_color.blue = 0.7 * toplevel->color.blue;
		toplevel_color.alpha = toplevel->color.alpha;

		ei_draw_frame(surface, toplevel_widget_rect, toplevel_color, ei_relief_none, &toplevel_clipper);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, toplevel_clipper);

		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;

		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;

		while (child) {
			if (child == widget->children_head) {
				int size_resize = 0.06 * widget->screen_location.size.height;
				child->screen_location.size.height = size_resize;
				child->screen_location.size.width = size_resize;
				ei_point_t bottom_right_resize;
				bottom_right_resize.x =
					widget->screen_location.top_left.x + widget->screen_location.size.width;
				bottom_right_resize.y =
					widget->screen_location.top_left.y + widget->screen_location.size.height;
				child->screen_location.top_left = ei_point(bottom_right_resize.x - size_resize,
									   bottom_right_resize.y - size_resize);
			} else if (child == widget->children_head->next_sibling) {
				child->screen_location.size.height = 0.7 * toplevel_widget_rect.size.height;
				child->screen_location.size.width = child->screen_location.size.height;
				child->screen_location.top_left = ei_point_add(toplevel_widget_rect.top_left, ei_point(0.95 * widget->screen_location.size.width,  0.2 * toplevel_widget_rect.size.height));
			} else {
				child->screen_location = ei_rect_intersect(*widget->content_rect, child->screen_location);
			}

			child->content_rect = &child->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t txt_clipper = ei_rect_intersect(toplevel_widget_rect, toplevel_clipper);

		if (clipper) {
			txt_clipper = ei_rect_intersect(txt_clipper, *clipper);
		}

		if (toplevel->title) {
			ei_surface_t txt_surface = hw_text_create_surface(toplevel->title, ei_default_font, ei_font_default_color);
			ei_rect_t txt_rect = hw_surface_get_rect(txt_surface);
			txt_rect.top_left = ei_anchor_text_img(&(ei_anchor_t) {ei_anc_west}, &txt_rect, &txt_clipper);
			ei_draw_text(surface, &txt_rect.top_left, (ei_const_string_t) toplevel->title, ei_default_font, (ei_color_t) { 0xDD, 0xDD, 0xDD, 0xFF }, &txt_clipper);
			hw_surface_free(txt_surface);
		}

		ei_impl_widget_draw_children(widget->children_head, surface, pick_surface, clipper);
	} else {
		ei_impl_widget_draw_children(widget->children_head->next_sibling, surface, pick_surface, clipper);
	}
}

void toplevel_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t*) widget;
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
				int dx = event->param.mouse.where.x - ((ei_point_t *) widget->user_data)->x;
				int dy = event->param.mouse.where.y - ((ei_point_t *) widget->user_data)->y;

				if (moving) {
					ei_placer_forget(widget);
					ei_toplevel_moving_update(widget, dx, dy);

					hw_surface_unlock(ei_app_root_surface());
					hw_surface_unlock(offscreen);

					hw_surface_update_rects(ei_app_root_surface(), NULL);

					hw_surface_lock(ei_app_root_surface());
					hw_surface_lock(offscreen);

					widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
				}

				moving = !moving;
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


void ei_toplevel_resizing_update(ei_widget_t widget, int dx, int dy)
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
		ei_widget_t next_child;

		if (child) {
			next_child = child->next_sibling;
		}

		while (child) {
			ei_toplevel_resizing_update(child, dx, dy);
			child = next_child;
			next_child = (child == NULL) ? NULL : child->next_sibling;
		}
	}
}

void ei_toplevel_moving_update(ei_widget_t widget, int dx, int dy)
{
	if (widget) {
		widget->screen_location.top_left.x += dx;
		widget->screen_location.top_left.y += dy;
		widget->content_rect->top_left.x += dx;
		widget->content_rect->top_left.y += dy;
		if (!strcmp(widget->wclass->name,"toplevel")){
			widget->placer_params = calloc(1, sizeof(struct ei_impl_placer_params_t));
			widget->placer_params->x = widget->screen_location.top_left.x;
			widget->placer_params->y = widget->screen_location.top_left.y;
			widget->placer_params->height = widget->screen_location.size.height;
			widget->placer_params->width = widget->screen_location.size.width;
		}
//		widget->placer_params->x = widget->screen_location.top_left.x;
//		widget->placer_params->y = widget->screen_location.top_left.y;
//		widget->placer_params->height = widget->screen_location.size.height;
//		widget->placer_params->width = widget->screen_location.size.width;
//		widget->placer_params->rel_x = 0;
//		widget->placer_params->rel_y = 0;
//		widget->placer_params->rel_width = NULL;
//		widget->placer_params->rel_height = NULL;

		ei_widget_t child = widget->children_head;
		ei_widget_t next_child;

		if (child) {
			next_child = child->next_sibling;
		}

		while (child) {
			ei_toplevel_moving_update(child, dx, dy);
			child = next_child;
			next_child = (child == NULL) ? NULL : child->next_sibling;
		}
	}
}

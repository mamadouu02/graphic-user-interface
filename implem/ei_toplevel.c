/**
 * @file	ei_toplevel.c
 *
 * @brief 	Toplevel class.
 *
 */

#include "ei_toplevel.h"
#include "ei_button.h"

extern ei_surface_t offscreen;

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
	/* à implémenter */
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &toplevel->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
	} else if (widget->placer_params) {
		ei_rect_t widget_rect;
		widget_rect = widget->screen_location;

		ei_rect_t clipper_frame = *widget->parent->content_rect;

		if (clipper) {
			clipper_frame = ei_rect_intersect(clipper_frame, *clipper);
		}

		ei_draw_frame(surface, widget_rect, toplevel->color, ei_relief_none, &clipper_frame);
		ei_draw_frame(pick_surface, widget_rect, widget->pick_color, ei_relief_none, &clipper_frame);

		ei_rect_t widget_rect_toplevel = widget_rect;

		widget_rect_toplevel.size.height = 0.1 * widget_rect.size.height;
		ei_color_t color_toplevel = (ei_color_t) {0.7 * toplevel->color.red, 0.7 * toplevel->color.green,
							  0.7 * toplevel->color.blue, toplevel->color.alpha};
		ei_draw_frame(surface, widget_rect_toplevel, color_toplevel, ei_relief_none, &clipper_frame);

		int size_resize = 0.06 * widget_rect.size.height;
		ei_point_t bottom_right_resize = ei_point(widget_rect.top_left.x + widget_rect.size.width, widget_rect.top_left.y + widget_rect.size.height);
		ei_point_t bottom_left_resize = ei_point(bottom_right_resize.x - size_resize, bottom_right_resize.y);
		ei_point_t top_left_resize = ei_point(bottom_right_resize.x - size_resize, bottom_right_resize.y - size_resize);
		ei_point_t top_right_resize = ei_point(bottom_right_resize.x, bottom_right_resize.y - size_resize);
		ei_point_t tab[4] = {bottom_right_resize, bottom_left_resize, top_left_resize, top_right_resize};

		ei_draw_polygon(surface, tab, 4, color_toplevel, &clipper_frame);
		ei_draw_polygon(pick_surface,  tab, 4, widget->pick_color, &clipper_frame);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, clipper_frame);

		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;

		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;

		while (child) {
			child->screen_location = ei_rect_intersect(*widget->content_rect,
								   widget->children_head->screen_location);
			child->content_rect = &widget->children_head->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t clipper_text = ei_rect_intersect(widget_rect_toplevel, clipper_frame);

		if (clipper) {
			clipper_text = ei_rect_intersect(clipper_text, *clipper);
		}

		if (toplevel->title) {
			ei_surface_t surface_text = hw_text_create_surface(toplevel->title, ei_default_font,
									   ei_font_default_color);
			ei_rect_t text_rect = hw_surface_get_rect(surface_text);
			text_rect.top_left = ei_anchor_text_img(&(ei_anchor_t) {ei_anc_west}, &text_rect,
								&clipper_text);
			ei_draw_text(surface, &text_rect.top_left, (ei_const_string_t) toplevel->title, ei_default_font,
				     (ei_color_t){0xDD,0xDD,0xDD,0xFF}, &clipper_text);
			hw_surface_free(surface_text);
		}
	}

	ei_impl_widget_draw_children(widget->children_head, surface, pick_surface, clipper);
}


void toplevel_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t*) widget;

	toplevel->color = ei_default_background_color;
	toplevel->border_width = 4;
	toplevel->title = "Toplevel";
	toplevel->closable = true;
	toplevel->resizable = ei_axis_both;
	toplevel->min_size = &(ei_size_t){160, 120};
}


bool ei_toplevel_handlefunc(ei_widget_t widget, struct ei_event_t* event)
{
	if (event->type == ei_ev_mouse_move && ei_event_get_active_widget() == widget) {
		widget->wclass->drawfunc( widget, ei_app_root_surface(), offscreen, NULL);
		ei_event_set_active_widget(NULL);
	} else if (event->type == ei_ev_mouse_buttondown && event->param.mouse.button == ei_mouse_button_left && ei_in_rect(event->param.mouse.where,widget->screen_location)) {
		ei_event_set_active_widget(widget);
	} else if (event->type == ei_ev_mouse_buttonup && event->param.mouse.button == ei_mouse_button_left && ei_in_rect(event->param.mouse.where, widget->screen_location)) {
		ei_event_set_active_widget(NULL);
	}
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
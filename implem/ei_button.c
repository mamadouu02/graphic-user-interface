/**
 * @file	ei_button.c
 *
 * @brief 	Button class.
 *
 */

#include "ei_button.h"

extern ei_surface_t offscreen;

ei_widget_t button_allocfunction(void)
{
	return calloc(1, sizeof(ei_impl_button_t));
}

void button_releasefunc(ei_widget_t widget)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;

	if (button->text)
		free(button->text);
	if (button->img)
		hw_surface_free(button->img);
	if (button->img_rect)
		free(button->img_rect);
}

void button_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &button->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
		ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
	} else if (widget->placer_params) {
		ei_rect_t widget_rect = widget->screen_location;
		ei_rect_t button_clipper = *widget->parent->content_rect;
		
		if (clipper) {
			button_clipper = ei_rect_intersect(button_clipper, *clipper);
		}

		ei_draw_button(surface, widget_rect, button->color, button->corner_radius, button->relief, &button->text, &button->text_font, &button->text_color, &button->text_anchor, &button_clipper);
		ei_draw_button(pick_surface, widget_rect, widget->pick_color, button->corner_radius, ei_relief_none, NULL, NULL, NULL, NULL, &button_clipper);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, button_clipper);
		
		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;
		
		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;
		
		while (child) {
			child->screen_location = ei_rect_intersect(child->screen_location, *widget->content_rect);
			child->content_rect = &child->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t img_clipper = new_screen_loc;
		
		if (clipper) {
			img_clipper = ei_rect_intersect(img_clipper, *clipper);
		}

		if (!button->text && button->img) {
			ei_rect_t img_rect = (clipper) ? ei_rect_intersect(*(button->img_rect), *clipper) : *(button->img_rect);
			img_rect.top_left = img_clipper.top_left;
			img_rect = ei_rect_intersect(img_rect, img_clipper);
			img_rect.top_left = ei_anchor_text_img(&button->img_anchor, &img_rect, &img_clipper);
			hw_surface_lock(button->img);
			ei_rect_cpy(surface, &img_rect, button->img, button->img_rect, true);
			hw_surface_unlock(button->img);
		}

		ei_impl_widget_draw_children(widget, surface, pick_surface, clipper);
	}
}

void button_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;
	button->color = ei_default_background_color;
	button->border_width = k_default_button_border_width;
	button->corner_radius = k_default_button_corner_radius;
	button->relief = (ei_relief_t) { ei_relief_raised };
	button->text_font = ei_default_font;
//	button->img_rect = NULL;
	button->text_color = ei_font_default_color;
	button->text_anchor = (ei_anchor_t) { ei_anc_center };
	button->img_anchor = (ei_anchor_t) { ei_anc_center };
}

bool ei_button_handlefunc(ei_widget_t widget, struct ei_event_t* event)
{
	hw_surface_lock(ei_app_root_surface());
	hw_surface_lock(offscreen);

	ei_impl_button_t *button = (ei_impl_button_t *) widget;

	switch (event->type) {
		case ei_ev_mouse_buttondown:
			if (ei_in_rect(event->param.mouse.where, widget->screen_location) && button->relief == ei_relief_raised) {
				button->relief = ei_relief_sunken;
				widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
				ei_event_set_active_widget(widget);
			}
			break;
		case ei_ev_mouse_buttonup:
			if (ei_in_rect(event->param.mouse.where, widget->screen_location) && button->relief == ei_relief_sunken) {
				button->relief = ei_relief_raised;
				widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);

				if (button->callback) {
					button->callback(widget, event, button->user_param);
					ei_widget_t root = ei_app_root_widget();
					ei_impl_app_run_children(root);
					root->wclass->drawfunc(root, ei_app_root_surface(), offscreen, NULL);
				}

				ei_event_set_active_widget(NULL);

				if (!strcmp(widget->parent->wclass->name, "toplevel") && widget == widget->parent->children_head->next_sibling) {
					ei_placer_forget(widget->parent);
				}
			}
			break;
		case ei_ev_mouse_move:
			if (ei_widget_pick(&event->param.mouse.where)->pick_id != widget->pick_id && button->relief == ei_relief_sunken) {
				button->relief = ei_relief_raised;
				widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
				ei_event_set_active_widget(NULL);
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

void ei_button_register(void)
{
	ei_widgetclass_t *button = calloc(1, sizeof(ei_widgetclass_t));
	strcpy(button->name, "button");
	button->allocfunc = &button_allocfunction;
	button->releasefunc = &button_releasefunc;
	button->drawfunc = &button_drawfunc;
	button->setdefaultsfunc = &button_setdefaultsfunc;
	button->handlefunc = &ei_button_handlefunc;
	ei_widgetclass_register(button);
}

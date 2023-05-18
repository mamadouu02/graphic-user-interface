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
	// ei_impl_button_t *button = (ei_impl_button_t *) widget;

	free(widget->user_data);
	free(widget->content_rect);
	// free(widget->destructor);
	free(widget->placer_params);
}

void button_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;

	if (widget->parent == NULL) {
		ei_fill(surface, &button->color, clipper);
		ei_fill(pick_surface, &widget->pick_color, clipper);
	} else if (widget->placer_params) {
		ei_rect_t widget_rect;
		widget_rect = widget->screen_location;

		ei_rect_t clipper_frame = *widget->parent->content_rect;
		
		if (clipper) {
			clipper_frame = ei_rect_intersect(clipper_frame, *clipper);
		}

		ei_draw_button(surface, widget_rect, button->color, button->corner_radius, button->relief, &button->text, &button->text_font, &button->text_color, &button->text_anchor, &clipper_frame);
		ei_draw_button(pick_surface, widget_rect, widget->pick_color, button->corner_radius, ei_relief_none, NULL, NULL, NULL, NULL, &clipper_frame);

		ei_rect_t new_screen_loc = ei_rect_intersect(widget_rect, clipper_frame);
		
		/* where to place children */
		widget->screen_location = new_screen_loc;
		widget->content_rect = &widget->screen_location;
		
		/* à changer pour tous les enfants aussi ! */
		ei_widget_t child = widget->children_head;
		
		while (child) {
			child->screen_location = ei_rect_intersect(*widget->content_rect, widget->children_head->screen_location);
			child->content_rect = &widget->children_head->screen_location;
			child = child->next_sibling;
		}

		ei_rect_t clipper_text_image = new_screen_loc;
		
		if (clipper) {
			clipper_text_image = ei_rect_intersect(clipper_text_image, *clipper);
		}

		if (!button->text && button->img) {
			ei_rect_t img_rect = (clipper) ? ei_rect_intersect(*(button->img_rect), *clipper) : *(button->img_rect);
			img_rect.top_left = clipper_text_image.top_left;
			img_rect = ei_rect_intersect(img_rect, clipper_text_image);
			img_rect.top_left = ei_anchor_text_img(&button->img_anchor, &img_rect, &clipper_text_image);
			ei_rect_cpy(surface, &img_rect, button->img, button->img_rect, true);
		}
	}

	ei_impl_widget_draw_children(widget->children_head, surface, pick_surface, clipper);
}

void button_setdefaultsfunc(ei_widget_t widget)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;
	button->color = ei_default_background_color;
	button->border_width = k_default_button_border_width;
	button->corner_radius = k_default_button_corner_radius;
	button->relief = (ei_relief_t) { ei_relief_raised };
	button->text_font = ei_default_font;
	button->text_color = ei_font_default_color;
	button->text_anchor = (ei_anchor_t) { ei_anc_center };
	button->img_anchor = (ei_anchor_t) { ei_anc_center };
}

bool ei_button_handlefunc(ei_widget_t widget, struct ei_event_t* event)
{
	hw_surface_lock(ei_app_root_surface());
	hw_surface_lock(offscreen);

	ei_impl_button_t *button = (ei_impl_button_t *) widget;

	if (event->type == ei_ev_mouse_move && (ei_widget_pick(&event->param.mouse.where)->pick_id != widget->pick_id) && button->relief == ei_relief_sunken) {
		button->relief = ei_relief_raised;
		widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
		ei_event_set_active_widget(NULL);
	} else if (event->type == ei_ev_mouse_buttonup && event->param.mouse.button == ei_mouse_button_left && ei_in_rect(event->param.mouse.where,widget->screen_location) && button->relief == ei_relief_sunken) {
		button->relief = ei_relief_raised;
		widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
		button->callback(widget, event, button->user_param);
		ei_event_set_active_widget(NULL);
	} else if (event->type == ei_ev_mouse_buttondown && event->param.mouse.button == ei_mouse_button_left && ei_in_rect(event->param.mouse.where, widget->screen_location) && button->relief == ei_relief_raised) {
		button->relief = ei_relief_sunken;
		widget->wclass->drawfunc(widget, ei_app_root_surface(), offscreen, NULL);
		ei_event_set_active_widget(widget);
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

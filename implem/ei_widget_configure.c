/**
 * @file	ei_widget_configure.c
 *
 * @brief 	API for widgets configuration.
 *
 */

#include "ei_widget_configure.h"
#include "ei_class.h"

void    ei_frame_configure      (ei_widget_t		widget,
			        ei_size_t*		requested_size,
			        const ei_color_t*	color,
			        int*			border_width,
			        ei_relief_t*		relief,
			        ei_string_t*		text,
			        ei_font_t*		text_font,
			        ei_color_t*		text_color,
			        ei_anchor_t*		text_anchor,
			        ei_surface_t*		img,
			        ei_rect_ptr_t*		img_rect,
			        ei_anchor_t*		img_anchor)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;
	frame->requested_size = (requested_size == NULL) ? frame->requested_size : requested_size;
	frame->color = (color == NULL) ? frame->color : color;
	frame->border_width = (border_width == NULL) ? frame->border_width : border_width;
	frame->relief = (relief == NULL) ? frame->relief : relief;
	frame->text = (text == NULL) ? frame->text : text;
	frame->text_font = (text_font == NULL) ? frame->text_font : text_font;
	frame->text_color = (text_color == NULL) ? frame->text_color : text_color;
	frame->text_anchor = (text_anchor == NULL) ? frame->text_anchor : text_anchor;
	frame->img = (img == NULL) ? frame->img : img;
	frame->img_rect = (img_rect == NULL) ? frame->img_rect : img_rect;
	frame->img_anchor = (img_anchor == NULL) ? frame->img_anchor : img_anchor;
}

void    ei_button_configure	(ei_widget_t		widget,
				ei_size_t*		requested_size,
				const ei_color_t*	color,
				int*			border_width,
				int*			corner_radius,
				ei_relief_t*		relief,
				ei_string_t*		text,
				ei_font_t*		text_font,
				ei_color_t*		text_color,
				ei_anchor_t*		text_anchor,
				ei_surface_t*		img,
				ei_rect_ptr_t*		img_rect,
				ei_anchor_t*		img_anchor,
				ei_callback_t*		callback,
				ei_user_param_t*	user_param)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;
	button->requested_size = requested_size;
	button->color = color;
	button->border_width = border_width;
	button->corner_radius = corner_radius;
	button->relief = relief;
	button->text = text;
	button->text_font = text_font;
	button->text_color = text_color;
	button->text_anchor = text_anchor;
	button->img = img;
	button->img_rect = img_rect;
	button->img_anchor = img_anchor;
	button->callback = callback;
	button->user_param = user_param;
}

void    ei_toplevel_configure	(ei_widget_t		widget,
				ei_size_t*		requested_size,
				ei_color_t*		color,
				int*			border_width,
				ei_string_t*		title,
				bool*			closable,
				ei_axis_set_t*		resizable,
				ei_size_ptr_t*		min_size)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;
	toplevel->requested_size = requested_size;
	toplevel->color = color;
	toplevel->border_width = border_width;
	toplevel->title = title;
	toplevel->closable = closable;
	toplevel->resizable = resizable;
	toplevel->min_size = min_size;
}


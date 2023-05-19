/**
 * @file	ei_widget_configure.c
 *
 * @brief 	API for widgets configuration.
 *
 */

#include "ei_widget_configure.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_toplevel.h"

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
	widget->requested_size = (requested_size) ? *requested_size : widget->requested_size;
	frame->color = (color) ? *color : frame->color;
	frame->border_width = (border_width) ? *border_width : frame->border_width ;
	frame->relief = (relief) ? *relief : frame->relief;
	frame->text = (text) ? *text : frame->text;
	frame->text_font = (text_font) ? *text_font : frame->text_font;
	frame->text_color = (text_color) ? *text_color : frame->text_color;
	frame->text_anchor = (text_anchor) ? *text_anchor : frame->text_anchor;
	frame->img = (img) ? *img : frame->img;
	frame->img_rect = (img_rect) ? *img_rect : frame->img_rect;
	frame->img_anchor = (img_anchor) ? *img_anchor : frame->img_anchor;
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
	widget->requested_size = (requested_size) ? *requested_size : widget->requested_size;
	button->color = (color) ? *color : button->color;
	button->border_width = (border_width) ? *border_width : button->border_width;
	button->corner_radius = (corner_radius) ? *corner_radius : button->corner_radius;
	button->relief = (relief) ? *relief : button->relief;
	button->text = (text) ? *text : button->text;
	button->text_font = (text_font) ? *text_font : button->text_font;
	button->text_color = (text_color) ? *text_color : button->text_color;
	button->text_anchor = (text_anchor) ? *text_anchor : button->text_anchor;
	button->img = (img) ? *img : button->img;
	button->img_rect = (img_rect) ? *img_rect : button->img_rect;
	button->img_anchor = (img_anchor) ? *img_anchor : button->img_anchor;
	button->callback = (callback) ? *callback : button->callback;
	button->user_param = (user_param) ? *user_param : button->user_param;
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
	widget->requested_size = (requested_size) ? *requested_size : widget->requested_size;
	toplevel->color = (color) ? *color : toplevel->color;
	toplevel->border_width = (border_width) ? *border_width : toplevel->border_width;
	toplevel->title = (title) ? *title : toplevel->title;
	toplevel->closable = (closable) ? *closable : toplevel->closable;
	toplevel->resizable = (resizable) ? *resizable : toplevel->resizable;
	toplevel->min_size = (min_size) ? *min_size : toplevel->min_size;
}

/**
 * @file	ei_widget_configure.c
 *
 * @brief 	API for widgets configuration.
 *
 */

#include "ei_widget_configure.h"

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
        /* A implémenter */
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
        /* A implémenter */
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
        /* A implémenter */
}

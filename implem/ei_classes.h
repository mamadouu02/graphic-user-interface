/**
 * @file	ei_classes.h
 *
 * @brief 	...
 *
 */

#ifndef EI_CLASSES_H
#define EI_CLASSES_H

#include "ei_implementation.h"

ei_widget_t 	frame_allocfunction 	(void);
void 		frame_releasefunc 	(ei_widget_t widget);
void 		frame_drawfunc 		(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);
void 		frame_setdefaultsfunc 	(ei_widget_t widget);

typedef struct ei_impl_frame_t {
    ei_impl_widget_t 	widget;

    ei_size_t		*requested_size;
    const ei_color_t	*color;
    int			*border_width;
    ei_relief_t		*relief;
    ei_string_t		*text;
    ei_font_t		*text_font;
    ei_color_t		*text_color;
    ei_anchor_t		*text_anchor;
    ei_surface_t	*img;
    ei_rect_ptr_t	*img_rect;
    ei_anchor_t		*img_anchor;
} ei_impl_frame_t;

typedef struct ei_impl_button_t {
    ei_impl_widget_t	widget;

    ei_size_t		*requested_size;
    const ei_color_t	*color;
    int			*border_width;
    int			*corner_radius;
    ei_relief_t		*relief;
    ei_string_t		*text;
    ei_font_t		*text_font;
    ei_color_t		*text_color;
    ei_anchor_t		*text_anchor;
    ei_surface_t	*img;
    ei_rect_ptr_t	*img_rect;
    ei_anchor_t		*img_anchor;
    ei_callback_t	*callback;
    ei_user_param_t	*user_param;
} ei_impl_button_t;

typedef struct ei_impl_toplevel_t {
    ei_impl_widget_t	widget;

    ei_size_t 	 	*requested_size;
    ei_color_t		*color;
    int 		*border_width;
    ei_string_t		* title;
    bool 		*closable;
    ei_axis_set_t	*resizable;
    ei_size_ptr_t 	*min_size;
} ei_impl_toplevel_t;

#endif

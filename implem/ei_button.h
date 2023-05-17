/**
 * @file	ei_button.h
 *
 * @brief 	Button class.
 *
 */

#ifndef EI_BUTTON_H
#define EI_BUTTON_H

#include "ei_implementation.h"
#include "ei_polygon.h"

/**
 * \brief	A button widget.
 */
typedef struct ei_impl_button_t {
    ei_impl_widget_t	widget;

    ei_color_t		color;
    int			border_width;
    int			corner_radius;
    ei_relief_t		relief;
    ei_string_t		text;
    ei_font_t		text_font;
    ei_color_t		text_color;
    ei_anchor_t		text_anchor;
    ei_surface_t	img;
    ei_rect_ptr_t	img_rect;
    ei_anchor_t		img_anchor;
    ei_callback_t	callback;
    ei_user_param_t	user_param;
} ei_impl_button_t;

ei_widget_t button_allocfunction(void);

void button_releasefunc(ei_widget_t widget);

void button_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

void button_setdefaultsfunc(ei_widget_t widget);

bool ei_button_handlefunc(ei_widget_t widget, struct ei_event_t* event);

void ei_button_register(void);

#endif

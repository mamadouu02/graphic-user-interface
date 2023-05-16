/**
 * @file	ei_toplevel.h
 *
 * @brief 	Toplevel class.
 *
 */

#ifndef EI_TOPLEVEL_H
#define EI_TOPLEVEL_H

#include "ei_implementation.h"

/**
 * \brief	A toplevel widget.
 */
typedef struct ei_impl_toplevel_t {
    ei_impl_widget_t	widget;
    ei_color_t		color;
    int 		border_width;
    ei_string_t		title;
    bool 		closable;
    ei_axis_set_t	resizable;
    ei_size_ptr_t 	min_size;
} ei_impl_toplevel_t;

#endif

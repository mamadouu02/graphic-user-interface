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

/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class: both the common attributes, that use
 *		\ref ei_widget_struct_size bytes, and the specific attributes of the class.
 *		After allocation, the function *must* initialize the memory to 0.
 *
 * @return			A block of memory big enough to represent a widget of this class,
 * 				with all bytes set to 0.
 */
ei_widget_t toplevel_allocfunction(void);

/**
 * \brief	A function that releases the memory used by a widget before it is destroyed.
 *		The memory used for the \ref ei_widget_t structure itself must *not* be freed by
 *		the function. It frees the *fields* of the structure, if necessary.
 *		Can be set to NULL in \ref ei_widgetclass_t if no memory is used by a class of widget.
 *
 * @param	widget		The widget which resources are to be freed.
 */
void toplevel_releasefunc(ei_widget_t widget);

/**
 * \brief	A function that draws a widget of a class.
 * 		The function must also draw the children of the widget.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		A locked surface where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void toplevel_drawfunc(ei_widget_t widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

/**
 * \brief	A function that sets the default values for a widget of this class.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void toplevel_setdefaultsfunc(ei_widget_t widget);

/**
 * @brief	A function that is called in response to an event. This function
 *		is internal to the library. It implements the generic behavior of
 *		a widget (for example a button looks sunken when clicked)
 *
 * @param	widget		The widget for which the event was generated.
 * @param	event		The event containing all its parameters (type, etc.)
 *
 * @return			A boolean telling if the event was consumed by the callback or not.
 *				If true, the library does not try to call other callbacks for this
 *				event. If false, the library will call the next callback registered
 *				for this event, if any.
 *				Note: The callback may execute many operations and still return
 *				false, or return true without having done anything.
 */
bool ei_toplevel_handlefunc(ei_widget_t widget, struct ei_event_t* event);

/**
 * @brief	Registers the button class to the program.
 */
void ei_toplevel_register(void);

#endif

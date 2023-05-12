/**
 * @file	ei_widget_attributes.c
 *
 * @brief 	API for accessing widgets attributes.
 *
 */

#include "ei_widget_attributes.h"

ei_widgetclass_t* ei_widget_get_class (ei_widget_t widget)
{
        /* A implémenter */
}

const ei_color_t* ei_widget_get_pick_color (ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

ei_widget_t ei_widget_get_parent (ei_widget_t widget)
{
        /* A implémenter */
}

ei_widget_t ei_widget_get_first_child (ei_widget_t widget)
{
        /* A implémenter */
}

ei_widget_t ei_widget_get_last_child (ei_widget_t widget)
{
        /* A implémenter */
}

ei_widget_t ei_widget_get_next_sibling (ei_widget_t widget)
{
        /* A implémenter */
}

void* ei_widget_get_user_data (ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

const ei_rect_t* ei_widget_get_screen_location (ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

const ei_rect_t* ei_widget_get_content_rect (ei_widget_t widget)
{
        /* A implémenter */
        return 0;
}

void ei_widget_set_content_rect	(ei_widget_t widget, const ei_rect_t* content_rect)
{
        /* A implémenter */
}

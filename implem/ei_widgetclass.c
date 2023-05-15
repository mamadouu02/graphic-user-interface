/**
 *  @file	ei_widgetclass.c
 *  @brief	Definition and registration of widget classes.
 *
 */

#include "ei_widgetclass.h"
#include "ei_class.h"

ei_widgetclass_t *classes = NULL;

size_t ei_widget_struct_size()
{
        return sizeof(ei_impl_widget_t);
}

void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
	widgetclass->next = classes;
	classes = widgetclass;
}

ei_widgetclass_t* ei_widgetclass_from_name(ei_const_string_t name)
{
	ei_widgetclass_t *widgetclass = calloc(1, sizeof(ei_widgetclass_t));

	if (!strcmp(name, "frame") || !strcmp(name, "button") || !strcmp(name, "toplevel")) {
		strcpy(widgetclass->name, name);

		if (!strcmp(name, "frame")) {
			widgetclass->allocfunc = &frame_allocfunction;
			widgetclass->releasefunc = &frame_releasefunc;
			widgetclass->drawfunc = &frame_drawfunc;
			widgetclass->setdefaultsfunc = &frame_setdefaultsfunc;
		}
	}

	return widgetclass;
}

/**
 *  @file	ei_widgetclass.c
 *  @brief	Definition and registration of widget classes.
 *
 */

#include "ei_widgetclass.h"
#include "ei_class.h"

size_t ei_widget_struct_size()
{
        return sizeof(ei_impl_widget_t);
}

void ei_widgetclass_register(ei_widgetclass_t* widgetclass)
{
	ei_widgetclass_t *previous_class = NULL;

	while (widgetclass != NULL) {
		ei_widgetclass_t *new_class = malloc(sizeof(ei_widgetclass_t));
		strcpy(new_class->name, widgetclass->name);
		new_class->allocfunc = widgetclass->allocfunc;
		new_class->releasefunc = widgetclass->releasefunc;
		new_class->drawfunc = widgetclass->drawfunc;
		new_class->setdefaultsfunc = widgetclass->setdefaultsfunc;
		new_class->geomnotifyfunc = widgetclass->geomnotifyfunc;
		new_class->handlefunc = widgetclass->handlefunc;
		widgetclass = widgetclass->next;

		if (previous_class != NULL) {
			previous_class->next = new_class;
		}

		previous_class = new_class;
	}
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

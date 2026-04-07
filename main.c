// Tanasi Robert - 315CD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void initialise(struct System *system)
{
	system->fi = fopen("tema1.in", "r");
	system->fo = fopen("tema1.out", "w");
	fscanf(system->fi, "%d", &system->n_units);
	system->units = malloc(system->n_units * sizeof(struct Unit));
	system->queue_available_units = create_list(NULL);
	for (int i = 0; i < system->n_units; i++) {
		fscanf(system->fi, "%d", &system->units[i].id);
		fscanf(system->fi, " ");
		fscanf(system->fi, "%c", &system->units[i].type);
		system->units[i].availability = 1;
		insert_rear(system->queue_available_units, &system->units[i]);
	}
	system->n_available_units = system->n_units;
	system->incidents = create_list(NULL);
	system->intervention_stack = create_list(NULL);
	system->q_high = create_list(NULL);
	system->q_mid = create_list(NULL);
	system->q_low = create_list(NULL);
}

int main()
{
	struct System *system = calloc(1, sizeof(struct System));
	initialise(system);
	int n_commands;
	fscanf(system->fi, "%d", &n_commands);
	fscanf(system->fi, "\n");
	for (int i = 0; i < n_commands; i++) {
		char *command = read_word(system);
		if (!strcmp(command, "ADD_INCIDENT"))
			add_incident(system);
		else if (!strcmp(command, "CHECK_UNITS_AVAILABILITY"))
			check_units_availability(system);
		else if (!strcmp(command, "DISPATCH"))
			dispatch(system);
		else if (!strcmp(command, "DISPATCH"))
			dispatch(system);
		else if (!strcmp(command, "UNDO_LAST_DISPATCH"))
			undo_last_dispatch(system);
		else if (!strcmp(command, "SOLVED_INCIDENT")) {
			int id;
			fscanf(system->fi, "%d", &id);
			solved_incident(id, system);
		}
		else if (!strcmp(command, "SHOW_UNIT")) {
			int id;
			fscanf(system->fi, "%d", &id);
			show_unit(id, system);
		}
		else if (!strcmp(command, "SHOW_INCIDENT")) {
			int id;
			fscanf(system->fi, "%d", &id);
			show_incident(id, system);
		}
		else if (!strcmp(command, "SHOW_INTERVENTIONS"))
			show_interventions(system);
		fscanf(system->fi, "\n");
		free(command);
	}
	free_system(system);
}
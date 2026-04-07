// Tanasi Robert - 315CD
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "utils.h"

#define INITIAL_SIZE 16

void print_error(struct System *system)
{
	fprintf(system->fo, "INVALID OPERATION! ERROR 404\n");
}

struct Node *create_list(void *info)
{
	struct Node *node = malloc(sizeof(struct Node));
	node->info = info;
	node->next = node;
	node->prev = node;
	return node;
}

int is_empty(struct Node *head)
{
	return head->next == head;
}

// used as enqueue / push to stack;
// for enqueue, we insert rear with the struct * as info
void insert_rear(struct Node *head, void *info)
{
	struct Node *node = create_list(info);
	node->next = head;
	node->prev = head->prev;
	head->prev->next = node;
	head->prev = node;
}

// needed for undo, even though you shouldn't add front to a queue
void insert_front(struct Node *head, void *info)
{
	struct Node *node = create_list(info);
	node->next = head->next;
	node->prev = head;
	head->next->prev = node;
	head->next = node;
}

// used for incident queues and unit queue, cast when needed
// delete from start of list
void *dequeue(struct Node *head)
{
	// list has only sentinel
	if (head->next == head)
		return NULL;
	struct Node *temp = head->next;
	head->next = temp->next;
	head->next->prev = head;
	void *info = temp->info;
	free(temp);
	return info;
}

// used only for interventions so it can be typed
// delete from end of list
struct Intervention *pop(struct Node *head)
{
	// list has only sentinel
	if (head->next == head)
		return NULL;
	struct Node *temp = head->prev;
	temp->prev->next = head;
	head->prev = temp->prev;
	struct Intervention *info = (struct Intervention *)temp->info;
	free(temp);
	return info;
}

char *read_word(struct System *system)
{
	char *word;
	int size = INITIAL_SIZE;
	word = malloc(size * sizeof(char));
	int c, i = 0;
	while ((c = fgetc(system->fi)) != EOF && c != ' ' && c != '\n') {
		if (i + 1 >= size) {
			char *temp = realloc(word, size * 2 * sizeof(char));
			if (temp)
				word = temp;
			size *= 2;
		}
		word[i++] = c;
	}
	word[i++] = '\0';
	// realloc to strlen + 1
	char *temp = realloc(word, i * sizeof(char));
	if (temp)
		word = temp;
	return word;
}

char *read_description(struct System *system)
{
	char *description;
	int size = INITIAL_SIZE;
	description = malloc(size * sizeof(char));
	int c, i = 0;
	fscanf(system->fi, "\"");
	while ((c = fgetc(system->fi)) != EOF && c != '"') {
		if (i + 1 >= size) {
			char *temp = realloc(description, size * 2 * sizeof(char));
			if (temp)
				description = temp;
			size *= 2;
		}
		description[i++] = c;
	}
	description[i++] = '\0';
	// realloc to strlen + 1
	char *temp = realloc(description, i * sizeof(char));
	if (temp)
		description = temp;
	return description;
}

// for undo
void add_front_incident(struct Incident *incident, struct System *system)
{
	strcpy(incident->status, "queued");
	// insert in proper queue
	if (strcmp(incident->priority, "high") == 0)
		insert_front(system->q_high, incident);
	else if (strcmp(incident->priority, "medium") == 0)
		insert_front(system->q_mid, incident);
	else if (strcmp(incident->priority, "low") == 0)
		insert_front(system->q_low, incident);
}

void enqueue_incident(struct Incident *incident, struct System *system)
{
	strcpy(incident->status, "queued");
	// insert in proper queue
	if (strcmp(incident->priority, "high") == 0)
		insert_rear(system->q_high, incident);
	else if (strcmp(incident->priority, "medium") == 0)
		insert_rear(system->q_mid, incident);
	else if (strcmp(incident->priority, "low") == 0)
		insert_rear(system->q_low, incident);
}

// 2.1
void add_incident(struct System *system)
{
	struct Incident *incident = malloc(sizeof(struct Incident));
	fscanf(system->fi, "%d", &incident->id);
	char priority[7], *description;
	fscanf(system->fi, "%s", priority);
	fscanf(system->fi, " ");
	description = read_description(system);
	strcpy(incident->priority, priority);
	incident->description = description;
	// insert in incident list
	insert_rear(system->incidents, incident);
	enqueue_incident(incident, system);
}

// 2.2
void check_units_availability(struct System *system)
{
	int size = system->n_available_units;
	fprintf(system->fo, "Number of available units: %d\n", size);
}

void add_intervention(struct System *system, struct Node *q)
{
	struct Incident *incident;
	struct Unit *unit;
	incident = (struct Incident *)dequeue(q);
	unit = (struct Unit *)dequeue(system->queue_available_units);
	if (!unit) {
		print_error(system);
		return;
	}
	strcpy(incident->status, "intervened");
	unit->availability = 0;
	system->n_available_units--;
	struct Intervention *intervention = malloc(sizeof(struct Intervention));
	intervention->unit = unit;
	intervention->incident = incident;
	insert_rear(system->intervention_stack, intervention);
}

// 2.3
void dispatch(struct System *system)
{
	if (!is_empty(system->q_high)) { 
		add_intervention(system, system->q_high);
		return;
	}
	if (!is_empty(system->q_mid)) {
		add_intervention(system, system->q_mid);
		return;
	}
	if (!is_empty(system->q_low)) { 
		add_intervention(system, system->q_low);
		return;
	}
	print_error(system);
}

// 2.4
void undo_last_dispatch(struct System *system)
{
	while (!is_empty(system->intervention_stack)) {
		struct Intervention *intervention = pop(system->intervention_stack);
		struct Incident *incident = intervention->incident;
		if (!strcmp(incident->status, "intervened")) {
			add_front_incident(incident, system);
			strcpy(incident->status, "queued");
			intervention->unit->availability = 1;
			system->n_available_units++;
			insert_rear(system->queue_available_units, intervention->unit);
			free(intervention);
			return;
		}
		free(intervention);
	}
	print_error(system);
}

//2.5
void solved_incident(int id, struct System *system)
{
	struct Node *head = system->intervention_stack;
	struct Node *current = head->next;
	struct Intervention *intervention;
	// iterate and find desired incident
	while (current != head) {
		intervention = (struct Intervention *)current->info;
		if (intervention->incident->id == id) {
			if (strcmp(intervention->incident->status, "intervened") != 0){
				print_error(system);
				return;
			}
			strcpy(intervention->incident->status, "solved");
			intervention->unit->availability = 1;
			system->n_available_units++;
			insert_rear(system->queue_available_units, intervention->unit);
			return;
		}
		current = current->next;
	}
	print_error(system);
}

//2.6
void show_unit(int id, struct System *system)
{
	for (int i = 0; i < system->n_units; i++)
		if (system->units[i].id == id) {
			struct Unit *unit = &system->units[i];
			char availability[12];
			if (unit->availability)
				strcpy(availability, "available");
			else
				strcpy(availability, "unavailable");
			fprintf(system->fo, "Unit %d is type %c and is %s\n",
					id, unit->type, availability);
			return;
		}
	print_error(system);
}

// 2.7
void show_incident(int id, struct System *system)
{
	struct Node *head = system->incidents;
	struct Node *current = head->next;
	struct Incident *incident;
	while (current != head) {
		incident = (struct Incident *)current->info;
		if (incident->id == id) {
			fprintf(system->fo, "Incident %d has %s priority, the following "
					"description: \"%s\" and is %s\n", id, incident->priority,
					incident->description, incident->status);
			return;
		}
		current = current->next;
	}
	print_error(system);
}

// 2.8
void show_interventions(struct System *system)
{
	if (is_empty(system->intervention_stack)) {
		fprintf(system->fo, "No intervention has been initiated\n");
		return;
	}
	struct Node *head = system->intervention_stack;
	struct Node *current = head->next;
	struct Intervention *intervention;
	while (current != head) {
		intervention = (struct Intervention *)current->info;
		fprintf(system->fo, "Incident %d was assigned to unit %d, and has the"
				" following status: \"%s\"\n", intervention->incident->id, 
				intervention->unit->id, intervention->incident->status);
		current = current->next;
	}
}

void free_list(struct Node *head, int want_free, int is_incident)
{
	struct Node *current = head;
	struct Node *temp;
	do {
		temp = current;
		current = current->next;
		if (is_incident) {
			// don't access null
			if (temp != head) {
				struct Incident *incident = temp->info;
				free(incident->description);
			}
		}
		if (want_free)
			free(temp->info);
		free(temp);
	} while(current != head);
}

void free_system(struct System *system)
{
	fclose(system->fi);
	fclose(system->fo);
	free(system->units);
	// free the data only once 
	// (from unit vector, incident and intervention list)
	free_list(system->incidents, 1, 1);
	free_list(system->intervention_stack, 1, 0);
	free_list(system->q_high, 0, 0);
	free_list(system->q_mid, 0, 0);
	free_list(system->q_low, 0, 0);
	free_list(system->queue_available_units, 0, 0);
	free(system);
}

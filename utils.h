// Tanasi Robert - 315CD
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// static vector
struct Unit {
	int id, availability;
	char type;
};

// we will also use a queue for units

// double linked circular with sentinel
// 3 queues (add rear, front, delete front)
struct Incident {
	int id;
	char priority[7];
	char *description;
	char status[11];
};

// double linked circular with sentinel
// stack (add rear, top, delete rear)
struct Intervention {
	struct Incident *incident;
	struct Unit *unit;
};

// all null on init
struct System {
	FILE *fi, *fo;
	// dynamically allocated vector of units
	struct Unit *units;
	int n_units;
	// stack containing struct Incident *
	struct Node *incidents;
	// stack containing struct Intervention *
	struct Node *intervention_stack;
	// queues containing struct Incident *
	struct Node *q_high, *q_mid, *q_low;
	// queue containing struct Unit *
	struct Node *queue_available_units;
	int n_available_units;
};

struct Node {
	void *info;
	struct Node *next, *prev;
};

// prints 404 not found
void print_error(struct System *system);
// creates a list of any type
struct Node *create_list(void *info);
int is_empty(struct Node *head);
// used as enqueue / push to stack;
// for enqueue, we insert rear with the struct * as info
void insert_rear(struct Node *head, void *info);
// used for incident queues and unit queue, cast when needed
void *dequeue(struct Node *head);
// used only for interventions so it can be typed
struct Intervention *pop(struct Node *head);
// reads a word (used for commands)
char *read_word(struct System *system);
char *read_description(struct System *system);
void enqueue_incident(struct Incident *incident, struct System *system);
void add_incident(struct System *system);
void check_units_availability(struct System *system);
void add_intervention(struct System *system, struct Node *q);
void dispatch(struct System *system);
void undo_last_dispatch(struct System *system);
void solved_incident(int id, struct System *system);
void show_unit(int id, struct System *system);
void show_incident(int id, struct System *system);
void show_interventions(struct System *system);
void free_system(struct System *system);

#endif
/*

Authors: Ben Richmond and Christian Black
Assignment no.: 2
Date: 11/4/22
Name of File: List.c
Short Description: contains the functions needed for the BurstList struct to operate

*/

#include "Data.h"

/*

	Name of Function: remove
	Parameters being given: BurstList (Current pointer located where the node
		is to be removed)
	output: none
	Description: removes the current pointer of the BurstList and sets it to the current
		pointer to the previous one in the list

*/
void removeBurst(BurstList* list) {

	if (list->current != list->current->next) {
		list->current->next->prev = list->current->prev;
		list->current = list->current->prev;
	}
	else
		list->current = NULL;
	list->length= list->length-1;
}

/*

	Name of Function: add
	Input: Pointer to the list and the burst to be added to the list
	Output: None
	Description: Corrects references to add a new Burst to an empty or full BurstList

*/

void addBurst(BurstList* list, Burst *burst) {

	if (list->current == NULL)
	{

		list->current = burst;
		burst->next = burst;
		burst->prev = burst;

	}
	else
	{

		burst->prev = list->current->prev;
		list->current->prev->next = burst;
		list->current->prev = burst;
		burst->next = list->current;

	}

	list->length = list->length + 1;

}

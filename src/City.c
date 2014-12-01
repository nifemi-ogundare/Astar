/*
 * City.c
 *
 *  Created on: Nov 15, 2014
 *      Author: nifemi
 */

#include "City.h"
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/** Compare two cities based on the total distance from start to goal through each city
 * @param c1 the first city to compare
 * @param c2 the second city to compare
 * @return -1 if c1 < c2
 * @return 0 if equal
 * @return 1 if c1 > c2
 */
int compCity (void *c1, void *c2) {
	int a = totalDistance((City*)c1);
	int b = totalDistance((City*)c2);

	if (a<b) {
		return -1;
	}else{
		return a>b;
	}
}

/** Display a city - name (distance from start of path)
 * @param c the city to display
 */
void prCity(void *c) {
	printf("%s (%d)\n", (((City *)c)->name), (((City *)c)->distFromStart));
	return;
}

/** Compare two neighbors based on the distance from the parent city
 * @param n1 the first neighbor to compare
 * @param n2 the second neighbor to compare
 * @return -1 if n1 < n2
 * @return 0 if equal
 * @return 1 if n1 > n2
 */
int compNeighbors (void *n1, void *n2) {
	int a = ((Neighbor *)n1)->distance;
	int b = ((Neighbor *)n2)->distance;

	if (a<b) {
		return -1;
	}else{
		return a>b;
	}
}

/** Load the map as a list of elements, with content from a given text file.
 * @param filename the name of the map file
 * @return a pointer to the complete map i.e a list of cities
 */
List* loadMap(char* filename) {
	List *map = newList(compCity,prCity);
	FILE *f = NULL;
	char *line = malloc (200*sizeof(line));
	int i;

	f = fopen(filename,"r");
	if (!f) {
		printf("%s\n", "file not found");
		exit(EXIT_FAILURE);
	}

	City *city = NULL;
	City *currCity = NULL;
	char *ch = NULL;
	char *words[10];
	char *tmp;
	int nwords;

	/*go through the file line by line
	 * count the words in each line (nwords), store them in a words[] array*/
	while (fgets(line, 100, f)) {
		ch = &line[0];
		nwords = 0;
		for(i=0;i<10;i++) {
			while(isspace(*ch)) {
				ch++;
			}
			if(*ch == '\0') {
				break;
			}
			words[nwords] = ch;
			nwords++;
			while(!isspace(*ch) && *ch != '\0') {
				ch++;
			}
			if(*ch == '\0') {
				break;
			}
			*ch = '\0';
			ch++;
			i++;
		}

		tmp = malloc(strlen(words[0]) + 1);
		strcpy(tmp, words[0]);
		city = findCity(tmp, map);

		/*make certain the current line is not empty by checking nwords
		 * check that the city exits. create it and add it to the map if it does not*/
		if ((city == NULL) && (nwords > 0)) {
			city = newCity(tmp);
			addList(map, city);
		}

		/*currCity determines the city whose neighbors are currently being added
		 * currCity changes when the line has 3 words, indicating the beginning of a full city definition*/
		if (nwords == 3) {
			city->longitude = atoi(words[1]);
			city->latitude = atoi(words[2]);
			currCity = findCity(tmp, map);
		}

		/*malloc the list of neighbors and add to the list when a neighbor is encountered*/
		if (nwords == 2) {
			Neighbor *n = (Neighbor*) malloc(sizeof(Neighbor));
			n->city = findCity(tmp, map);
			n->distance = atoi(words[1]);
			addList(currCity->neighbors, n);
		}
	}
	free(tmp);
	free(line);
	fclose(f);
	return map;
}

/** Free the memory used during map creation
 * @param map the map to delete
 */
void delMap(List *map) {
	int i;
	Node *n = map->head;
	for (i=0; i<map->nelts; i++) {
		delCity((City*)n->val);
		n = n->next;
	}
	delList(map);
	return;
}

/** Free the memory used during city creation
 * @param c the city to delete
 */
void delCity(City* c) {
	delNeighbors(c);
	free(c->name);
	free(c);
	return;
}

/** Free the memory used during neighbor creation
 * @param c the city whose neighbors will be deleted
 */
void delNeighbors(City* c) {
	int i;
	Node *n = c->neighbors->head;
	for (i=0; i<c->neighbors->nelts; i++) {
		free(n->val);
		n = n->next;
	}
	delList(c->neighbors);
	return;
}

/** Create a new city from a given name, initialise the fields
 * @param name the name of the new city
 * @return a pointer to the created city
 */
City* newCity(char* name) {
	City *result = (City *) malloc(sizeof(City));
	result->name = name;
	result->distFromStart = INT_MAX;
	result->distToGoal = 0;
	result->latitude = 0;
	result->link = NULL;
	result->longitude = 0;
	result->neighbors = newList(compNeighbors, prCity);
	return result;
}

/** Find a city in the map by its name
 * @param name the name of the city to find
 * @param map the map to search
 * @return a pointer to the city, if it exists in the map
 * @return NULL if it doesnt
 */
City* findCity(char* name, List *map) {
	int i;
	Node *p = map->head;
	char *tmp = NULL;

	for (i=0; i<map->nelts; i++) {
		tmp = ((City*)p->val)->name;

		if (strcmp(tmp, name) == 0) {
			return p->val;
		}
		p = p->next;
	}
	return NULL;
}

/** Heuristic function to estimate the distance between two cities
 * Uses Euclidean distance formula, where D = 0.4
 * Estimate should always be <= the straight line distance between the cities
 * @param c1 the first city
 * @param c2 the second city
 * @return the euclidean distance
 */
int estimateDistance(City *c1, City *c2) {
	int a = abs(c1->longitude - c2->longitude);
	int b = abs(c1->latitude - c2->latitude);

	return 0.4 * sqrt(a*a + b*b);
}

/** Display the resolved path using the backpath pointer
 * @param c the city at the end of the path
 */
void displayPath(City *c) {
	while (c != NULL) {
		prCity(c);
		c = c->link;
	}
	return;
}

/** Compute total path distance through the given city
 * @param c the given city
 */
int totalDistance(City *c)  {
	return c->distFromStart + c->distToGoal;
}

/*
 * aStarProject.c
 *
 *  Created on: Nov 14, 2014
 *      Author: nifemi
 */

#include <stdio.h>
#include "City.h"

/**Function to compute the estimated distance to the goal from a given city,
 * 	and add the city to the open list
 * @param c the given city
 * @param goal the goal city
 * @param open the open list
 */
void addToOpen(City *c, City *goal, List *open) {
	c->distToGoal = estimateDistance(c, goal);
	addList(open, c);
	return;
}

int main(int argc, char *argv[]) {
	if ( argc != 4 ) {
		printf( "usage: %s filename start goal\n", argv[0] );
	}else{
		char *filename = argv[1];
		List *map = loadMap(filename);
		City *start = findCity(argv[2], map);
		City *goal = findCity(argv[3],  map);

		if (start == NULL || goal == NULL) {
			printf("%s\n", "invalid start and/or goal city");
			exit(EXIT_FAILURE);
		}

		List *open = newList(compCity, prCity);
		List *closed = newList(compCity, prCity);
		City *n;
		Node *node;
		Neighbor *s;
		int i;
		int success = 0;

		start->distFromStart = 0;
		addToOpen(start, goal, open);

		/* A* Algorithm ************************************************************************************/
		while (1) {
			/* End if the open list is empty*/
			if (lengthList(open) == 0) {
				printf("%s\n", "Failed");
				break;
			}else{
				n = (City*)open->head->val;
				remFromList(open, n);
				addList(closed, n);
			}

			/* End if the goal city has been reached*/
			if (compCity(n, goal) == 0) {
				success = 1;
				break;
			}

			node = n->neighbors->head;
			int g;

			for (i=0;i<n->neighbors->nelts;i++) {
				s = (Neighbor*)(node->val);
				g = n->distFromStart + s->distance;
				if ((findCity(s->city->name, open) || findCity(s->city->name, closed)) || (g > s->city->distFromStart)) {
					node = node->next;
				}else{
					if (findCity(s->city->name, open)) {
						remFromList(open, s->city);
					}
					if (findCity(s->city->name, closed)) {
						remFromList(closed, s->city);
					}
					s->city->distFromStart = g;
					s->city->link = n;
					addToOpen(s->city, goal, open);
					node = node->next;
				}
			}
		}

		/* only display the path if the algorithm was successful*/
		if (success) {
			printf("The best path, distance from start in brackets:\n");
			displayPath(goal);
		}

		/*clean up*/
		delMap(map);
		delList(open);
		delList(closed);
	}
	return 0;
}

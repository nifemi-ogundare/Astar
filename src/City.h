/*
 * City.h
 *
 *  Created on: Nov 15, 2014
 *      Author: nifemi
 */

#ifndef CITY_H_
#define CITY_H_

#include <stdlib.h>
#include "List.h"

typedef struct City {
  char	*name;
  int longitude, latitude;
  int distFromStart, distToGoal;
  struct City *link; /*backpath pointer*/
  List *neighbors;
} City;

typedef struct Neighbor {
	City *city;
	int distance;
} Neighbor;

/** Compare two cities based on the total distance from start to goal through each city
 * @param c1 the first city to compare
 * @param c2 the second city to compare
 * @return -1 if c1 < c2
 * @return 0 if equal
 * @return 1 if c1 > c2
 */
int compCity (void*, void*);

/** Display a city [name (distance from start of path)]
 * @param c the city to display
 */
void prCity(void*);

/** Compare two neighbors based on the distance from the parent city
 * @param n1 the first neighbor to compare
 * @param n2 the second neighbor to compare
 * @return -1 if n1 < n2
 * @return 0 if equal
 * @return 1 if n1 > n2
 */
int compNeighbors (void*, void*);

/** Load the map as a list of elements, with content from a given text file.
 * @param filename the name of the map file
 * @return a pointer to the complete map i.e a list of cities
 */
List* loadMap(char*);

/** Free the memory used during map creation
 * @param map the map to delete
 */
void delMap(List*);

/** Free the memory used during city creation
 * @param c the city to delete
 */
void delCity(City*);

/** Free the memory used during neighbor creation
 * @param c the city whose neighbors will be deleted
 */
void delNeighbors(City*);

/** Create a new city from a name, initialise the fields
 * @param name the name of the new city
 * @return a pointer to the created city
 */
City* newCity(char*);

/** Find a city in the map by its name
 * @param name the name of the city to find
 * @param map the map to search
 * @return a pointer to the city, if it exists in the map
 * @return NULL if it doesnt
 */
City* findCity(char*, List*);

/** Heuristic function to estimate the distance between two cities
 * Uses Euclidean distance formula, where D = 0.4
 * Estimate should always be <= the straight line distance between the cities
 * @param c1 the first city
 * @param c2 the second city
 * @return the euclidean distance
 */
int estimateDistance(City*, City*); //h function

/** Display the resolved path using the backpath pointer
 * @param c the city at the end of the path
 */
void displayPath(City*);

/** Compute total path distance through the given city
 * @param c the given city
 */
int totalDistance(City*); //f function

#endif /* CITY_H_ */

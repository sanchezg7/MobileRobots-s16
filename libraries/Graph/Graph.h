#ifndef GRAPH_H
#define GRAPH_H

#include <queue>
#include <stack>
#include <iostream>

using namespace std;

typedef enum {WHITE, GRAY, BLACK} allcolors;
typedef struct Cell 
{
	int id;
	int neighbors[4];
} Cell;

class Graph
{
	public:
		Graph();
		void makeGraph(int maze);
		void makeMaze1();
		void makeMaze2();
		void makeMaze3();
		void fillInNeighbors(int cell, int a, int b, int c, int d);
		void findPath();
		void breadthFirstSearch(int u,int exit);
		void printPath();
	private:
		int entry;
		int exit;
		Cell mazeCell[17];
		int parent[17];
		allcolors color[17];//holds the color for each index to mark off when visited
		queue<int> q;//queue for breadth first search
		stack<int> hold;
};

#endif

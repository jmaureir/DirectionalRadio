/*
 * NeighborGraph.h
 *
 *  Created on: Nov 2, 2009
 *      Author: jcm
 */

#ifndef NEIGHBORGRAPH_H_
#define NEIGHBORGRAPH_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <map>

#include "Coord.h"

class NeighborGraph {

	// forward declaration
public:
	class NeighborGraphNode;
	typedef std::list<NeighborGraphNode*> NodeList;
private:
	class NeighborGraphHeader;
	typedef std::multimap<double,NeighborGraphHeader*> Axe;

public:
	class NeighborGraphNode {
	public:
		Coord _pos;
		Coord _pos_prev;

		int node_id;

		NeighborGraphHeader* container;
		NeighborGraphHeader* boundary[4];

		// influence area offset from host position (0: left_x, 1:right_y, 2:right_x, 3: left_y)
		double influence_offset[4];

		NeighborGraphNode(Coord p);

		void createBoundaryNodes();

		Coord getBoundaryCoord(int seq);
		Coord getPreviousBoundaryCoord(int seq);

		double getPosition(int axe);
		double getPreviousPosition(int axe);

		bool isInRange(Coord pos);
	};

private:

	class NeighborGraphHeader {
	public:
		enum Type {
			CONTAINER,
			BOUNDARY,
		};

		Type type;
		NeighborGraphNode* graph_node;        // graph's node

		Axe::iterator it[2];

		NeighborGraphHeader() {
			this->type       = CONTAINER;
			this->graph_node = NULL;
		}

		double getPosition(int dim);
		double getPreviousPosition(int dim);

		Coord getPosition();

	};

	Axe axes[2];

	// TODO: TO Remove this
	std::vector<NeighborGraphNode*> nodes;

	Axe::iterator findHostInAxe(int axe,double pos,NeighborGraphHeader* container);
public:

	NeighborGraph();
	virtual ~NeighborGraph();

	bool insert(NeighborGraphNode* h) {

		// create the boundary nodes for the host
		h->createBoundaryNodes();

		// TODO: Remove it
		this->nodes.push_back(h);

		// host position
		double pos[] = {h->_pos.x, h->_pos.y};

		// insert the host (container) and its boundaries on the axes
		for(int axe=0;axe<2;axe++) {
			for(int b=0;b<4;b++) {
				NeighborGraphHeader* boundary = h->boundary[b];

				//std::cout << "inserting boundary axe " << axe << " pos " << boundary->getPosition(axe) << std::endl;

				boundary->it[axe] = this->axes[axe].insert(std::make_pair<double,NeighborGraphHeader*>(boundary->getPosition(axe),boundary));
			}

			//std::cout << "inserting container axe " << axe << " pos " << pos[axe] << std::endl;

			h->container->it[axe] = this->axes[axe].insert(std::make_pair<double,NeighborGraphHeader*>(pos[axe],h->container));
		}

		return true;
	}

	void updateNodePosition(NeighborGraphNode* h, Coord new_pos);
	void getNeighborsForNode(NeighborGraphNode* h, NodeList& neighbors, NodeList& toUpdate);

};


#endif /* NEIGHBORGRAPH_H_ */

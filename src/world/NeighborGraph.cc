/*
 * NeighborGraph.cpp
 *
 *  Created on: Nov 2, 2009
 *      Author: jcm
 */

#include "NeighborGraph.h"

NeighborGraph::NeighborGraph() {
	// TODO Auto-generated constructor stub

}

NeighborGraph::~NeighborGraph() {
	// TODO Auto-generated destructor stub
}

NeighborGraph::NeighborGraphNode::NeighborGraphNode(Coord p) {
	this->_pos = p;

	this->container = new NeighborGraphHeader();
	this->container->type = NeighborGraphHeader::CONTAINER;
	this->container->graph_node = this;

	// TODO: get this information from the radio
	this->influence_offset[0] = -200;
	this->influence_offset[1] = 200;
	this->influence_offset[2] = 200;
	this->influence_offset[3] = -200;
}

double NeighborGraph::NeighborGraphNode::getPosition(int axe) {
	double pos[] = {this->_pos.x,this->_pos.y};
	return pos[axe];
}

double NeighborGraph::NeighborGraphNode::getPreviousPosition(int axe) {
	double pos[] = {this->_pos_prev.x,this->_pos_prev.y};
	return pos[axe];
}

void NeighborGraph::NeighborGraphNode::createBoundaryNodes() {
	// polygon defined by a sequence of points, clock-wise
	// default case: an square (4 points)
	// 0, left, top
	// 1, right, top
	// 2, right, bottom
	// 3, left, bottom

	for(unsigned int i=0;i<4;i++) {
		this->boundary[i] = new NeighborGraphHeader();
		this->boundary[i]->type = NeighborGraphHeader::BOUNDARY;
		this->boundary[i]->graph_node = this;
	}
}

bool NeighborGraph::NeighborGraphNode::isInRange(Coord pos) {
	if (pos.x >= this->_pos.x + this->influence_offset[0] && pos.x <= this->_pos.x + this->influence_offset[2] &&
		pos.y >= this->_pos.y + this->influence_offset[3] && pos.y <= this->_pos.y + this->influence_offset[1]) {
		return true;
	}

	return false;
}

Coord NeighborGraph::NeighborGraphNode::getBoundaryCoord(int seq) {
	// i=0 => left,top        offsets (0,1)
	// i=1 => right,top       offsets (2,1)
	// i=2 => right,bottom    offsets (2,3)
	// i=3 => left,bottom     offsets (0,3)

	//std::cout << "getBoundaryCoord " << seq << " this " << this << " " << this->node_id << " pos " << this->_pos << endl;

	if (seq==0) {
		return Coord(this->_pos.x + this->influence_offset[0],this->_pos.y + this->influence_offset[1]);
	}
	if (seq==1) {
		return Coord(this->_pos.x + this->influence_offset[2],this->_pos.y + this->influence_offset[1]);
	}
	if (seq==2) {
		return Coord(this->_pos.x + this->influence_offset[2],this->_pos.y + this->influence_offset[3]);
	}
	if (seq==3) {
		return Coord(this->_pos.x + this->influence_offset[0],this->_pos.y + this->influence_offset[3]);
	}

	return Coord();
}

Coord NeighborGraph::NeighborGraphNode::getPreviousBoundaryCoord(int seq) {
	// i=0 => left,top        offsets (0,1)
	// i=1 => right,top       offsets (2,1)
	// i=2 => right,bottom    offsets (2,3)
	// i=3 => left,bottom     offsets (0,3)

	//std::cout << "getBoundaryCoord " << seq << " this " << this << " " << this->node_id << " pos " << this->_pos << endl;

	if (seq==0) {
		return Coord(this->_pos_prev.x + this->influence_offset[0],this->_pos_prev.y + this->influence_offset[1]);
	}
	if (seq==1) {
		return Coord(this->_pos_prev.x + this->influence_offset[2],this->_pos_prev.y + this->influence_offset[1]);
	}
	if (seq==2) {
		return Coord(this->_pos_prev.x + this->influence_offset[2],this->_pos_prev.y + this->influence_offset[3]);
	}
	if (seq==3) {
		return Coord(this->_pos_prev.x + this->influence_offset[0],this->_pos_prev.y + this->influence_offset[3]);
	}

	return Coord();
}

double NeighborGraph::NeighborGraphHeader::getPosition(int dim) {
	// dim:0 = x
	// dim:1 = y
	if (this->graph_node!=NULL) {
		if (this->type == NeighborGraphHeader::CONTAINER) {
			return this->graph_node->getPosition(dim);
		}
		if (this->type == NeighborGraphHeader::BOUNDARY) {
			for(unsigned int i=0;i<4;i++) {
				if (this->graph_node->boundary[i] == this) {
					Coord p = this->graph_node->getBoundaryCoord(i);
					if (dim==0) {
						return p.x;
					} else {
						return p.y;
					}
				}
			}
		}
	}
	return -1;
}

double NeighborGraph::NeighborGraphHeader::getPreviousPosition(int dim) {
	// dim:0 = x
	// dim:1 = y
	if (this->graph_node!=NULL) {
		if (this->type == NeighborGraphHeader::CONTAINER) {
			return this->graph_node->getPreviousPosition(dim);
		}
		if (this->type == NeighborGraphHeader::BOUNDARY) {
			for(unsigned int i=0;i<4;i++) {
				if (this->graph_node->boundary[i] == this) {
					Coord p = this->graph_node->getPreviousBoundaryCoord(i);
					if (dim==0) {
						return p.x;
					} else {
						return p.y;
					}
				}
			}
		}
	}
	return -1;
}

Coord NeighborGraph::NeighborGraphHeader::getPosition() {
	if (this->graph_node!=NULL) {
		if (this->type == NeighborGraphHeader::CONTAINER) {
			return this->graph_node->_pos;
		}
		if (this->type == NeighborGraphHeader::BOUNDARY) {
			for(unsigned int i=0;i<4;i++) {
				if (this->graph_node->boundary[i] == this) {
					Coord p = this->graph_node->getBoundaryCoord(i);
					return p;
				}
			}
		}
	}
	return Coord();
}

NeighborGraph::Axe::iterator NeighborGraph::findHostInAxe(int axe,double pos,NeighborGraphHeader* container) {

	//std::cout << "findHostInAxe " << axe << " " << pos << " container type " << container->type <<  std::endl;

	//for(Axe::iterator it=this->axes[axe].begin();it!=this->axes[axe].end();it++) {
	//	std::cout << (*it).first << " ";
	//}
	//std::cout << std::endl;

	std::pair<Axe::iterator,Axe::iterator> ret;
	ret = this->axes[axe].equal_range(pos);

	//std::cout << "  find host in axe " << axe << " " << (*ret.first).first << " " << (*ret.second).first << std::endl;

	if (ret.first == this->axes[axe].end()) {
		// not found
		//std::cout << "  NOT FOUND " << std::endl;
		return this->axes[axe].end();
	}

	if (ret.first == ret.second) {
		// single hit
		//std::cout << "  single hits " << (*ret.first).first << std::endl;
		return ret.first;
	} else {
		// multiple hits, search the right container (complexity: linear)

		//std::cout << "  multiple hits" << std::endl;

		for(Axe::iterator it=ret.first;it!=ret.second;it++) {
			if ((*it).second == container) {
				//std::cout << "  multiple hits: FOUND! " << std::endl;
				return it;
			}
		}
		std::cout << "  multiple hits: NOT FOUND!" << std::endl;
		return this->axes[axe].end();
	}
}

void  NeighborGraph::updateNodePosition(NeighborGraphNode* h, Coord new_pos) {

	//std::cout << "updating node position node " << h->_pos << " container " << h->container->pos << endl;

	h->_pos_prev = h->_pos;
	h->_pos = new_pos;

	//double pos_h[] = {h->_pos.x,h->_pos.y};
	//double pos_c[] = {h->container->pos.x,h->container->pos.y};

	// update the container position
	//h->container->pos = h->_pos;
	//h->container->graph_node->_pos = h->_pos;

	for(int axe=0;axe<2;axe++) {
		Axe::iterator it = this->findHostInAxe(axe,h->getPreviousPosition(axe),h->container);
		//std::cout << "found " << pos_c[axe] << " host pos " << pos_h[axe] << std::endl;
		//std::cout << "removing " << (*it).first << std::endl;

		// place the container in the correct order (remove and insert)
		this->axes[axe].erase(it);
		h->container->it[axe] = this->axes[axe].insert(std::make_pair<double,NeighborGraphHeader*>(h->getPosition(axe),h->container));

		//std::cout << "axe " << axe << " container pos " << h->container->pos << " host pos " << h->_pos << " container " << h->container << " graph_node " << h->container->graph_node << " h " << h << std::endl;

		// reorder each boundary node
		for(int i=0;i<4;i++) {
			NeighborGraphHeader* bn = h->boundary[i];
			Axe::iterator it = this->findHostInAxe(axe,bn->getPreviousPosition(axe),bn);
			//std::cout << "found boundary " << bn->pos << " new pos " << pos_bn[axe] << std::endl;
			this->axes[axe].erase(it);
			bn->it[axe] = this->axes[axe].insert(std::make_pair<double,NeighborGraphHeader*>(bn->getPosition(axe),bn));

		}
	}
}

void NeighborGraph::getNeighborsForNode(NeighborGraphNode* h, NodeList& neighbors, NodeList& toUpdate) {

	//std::cout << "calculating neighbors for " << h->node_id << " " << h->_pos << " container pos " << h->container->pos << std::endl;

	double pos[] = {h->_pos.x,h->_pos.y};

	// this take 2*log(n) + c (c is the number of nodes within the same coordinate)
	//Axe::iterator its[] = {this->findHostInAxe(0,pos[0],h->container), this->findHostInAxe(1,pos[1],h->container)};

	Axe::iterator its[] = {h->container->it[0],h->container->it[1]};

	// search neighbors to each direction
	for(int i=0;i<4;i++) {
		int axe = i % 2;

		//for(Axe::iterator it=this->axes[axe].begin();it!=this->axes[axe].end();it++) {
		//	std::cout << (*it).first << "(" << (*it).second << ") ";
		//}
		//std::cout << std::endl;

		Axe::const_iterator it = its[axe];

		double limit = pos[axe] + h->influence_offset[i];

		double high_limit = 0;
		double low_limit = 0;

		if (axe==0) {
			high_limit = pos[1] + h->influence_offset[1];
			low_limit  = pos[1] + h->influence_offset[3];
		} else {
			high_limit = pos[0] + h->influence_offset[2];
			low_limit  = pos[0] + h->influence_offset[0];
		}

		// left
		if (h->influence_offset[i] < 0) {

			NeighborGraphHeader* current = (*it).second;
			while (current!=NULL && current->getPosition(axe) >= limit && it!=this->axes[axe].begin()) {
				if (current->type==NeighborGraphHeader::CONTAINER) {
					//std::cout << "current " << current << " axe " << axe << " left " <<  current->graph_node->node_id << " " << current->type << " " << current->getPosition() << " " << current->getPosition(axe) << " " << limit << " " << low_limit << " " << high_limit;
					if (current->getPosition(!axe) >= low_limit && current->getPosition(!axe) <= high_limit && current != h->container) {
						neighbors.push_back(current->graph_node);
						//std::cout << " in";
					}
				} else if (current->type==NeighborGraphHeader::BOUNDARY) {
					//std::cout << "current " << current << " axe " << axe << " left " <<  current->graph_node->node_id << " " << current->type << " " << current->getPosition() << " " << current->getPosition(axe) << " " << limit << " " << low_limit << " " << high_limit;
					if ((current->graph_node->isInRange(h->_pos_prev) != current->graph_node->isInRange(h->_pos))) {
						toUpdate.push_back(current->graph_node);
						//std::cout << " 2update";
					}

				}
				//std::cout << std::endl;
				it--;
				current = (*it).second;
			}
		} else {
			// right
			NeighborGraphHeader* current = (*it).second;

			while (current!=NULL && current->getPosition(axe) <= limit && it!=this->axes[axe].end()) {
				if (current->type==NeighborGraphHeader::CONTAINER) {
					//std::cout << "current " << current << " axe " <<  axe  << " right " <<  current->graph_node->node_id << " " << current->type << " " <<  current->getPosition() << " " << current->graph_node->_pos << " " << current->getPosition(axe) << " " << limit << " " << low_limit << " " << high_limit;
					if (current->getPosition(!axe) >= low_limit && current->getPosition(!axe) <= high_limit && current != h->container) {
						neighbors.push_back(current->graph_node);
						//std::cout << " in";
					}
				} else if (current->type==NeighborGraphHeader::BOUNDARY) {
					//std::cout << "current " << current << " axe " <<  axe  << " right " <<  current->graph_node->node_id << " " << current->type << " " <<  current->getPosition() << " " << current->graph_node->_pos << " " << current->getPosition(axe) << " " << limit << " " << low_limit << " " << high_limit;
					if ((current->graph_node->isInRange(h->_pos_prev) != current->graph_node->isInRange(h->_pos))) {
						toUpdate.push_back(current->graph_node);
						//std::cout << " 2update";
					}

				}
				//std::cout << std::endl;
				it++;
				current = (*it).second;
			}
		}
	}


	// make neighbors unique
	neighbors.sort();
	neighbors.unique();

	// make the toUpdate unique
	toUpdate.sort();
	toUpdate.unique();

	//std::cout << "neighbors size :" << neighbors.size() << " toUpdate size :" << toUpdate.size() << std::endl;
}

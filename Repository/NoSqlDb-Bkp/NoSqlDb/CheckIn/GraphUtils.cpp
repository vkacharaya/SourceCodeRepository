/////////////////////////////////////////////////////////////////////////////
// GraphUtils.cpp - This package is responsible for creating graph using   //
//				    suign the db elements and alos contains tarjans algorithm
//					for finding the strongly connected components
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// copyright © Vishnu Karthik Ravindran, 2018                              //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2010                             //
// Platform:    HP Laptop , Core i5, Windows 10                            //
// Application: Course Projects, 2018                                      //
// Author:      Vishnu Karthik Ravindran, Syracuse University              //
//													                       //
/////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module provides class, GraphUtils
*
* The GraphUtils class supports creating graph
* using the db elements and also contains tarjans algorithm
* for finding the strongly connected components

* Public Interface:
* =================
* void GraphUtils::createGraph(DbCore<PayLoad>& map, std::unordered_map<std::string, std::string> &idMap)
* std::unordered_map<int, std::vector<int>> GraphUtils::SCC()
*
* Required Files:
* ===============
* Graph.h, GraphUtils.h, GraphUtils.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include "GraphUtils.h"

using namespace GraphProcessing;
using namespace NoSqlDb;

/////////////////////////////////////////////////////////
// creates graph using db elements
void GraphUtils::createGraph(DbCore<PayLoad>& map, std::unordered_map<std::string, std::string> &idMap)
{
	// sets the vertex size
	this->_vertex = map.size();
	for (auto item : map)
	{
		_graph.addNode(Sptr(new Node(item.first)));
	}
	for (auto item : map)
	{
		for (auto child : item.second.children())
		{
			_graph.addEdge(item.first, child);
			if (idMap.size() > 0)
				_adj[std::stoi(idMap[item.first])].push_back(std::stoi(idMap[child]));
		}
	}
}

// A recursive function that finds and prints strongly connected
// components using DFS traversal
void GraphUtils::SCCUtil(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[])
{
	// A static variable is used for simplicity, we can avoid use
	// of static variable by passing a pointer.
	static int time = 0;

	// Initialize discovery time and low value
	disc[u] = low[u] = ++time;
	st->push(u);
	stackMember[u] = true;

	// Go through all vertices adjacent to this
	std::list<int>::iterator i;
	for (i = _adj[u].begin(); i != _adj[u].end(); ++i)
	{
		int v = *i;  // v is current adjacent of 'u'
		 // If v is not visited yet, then recur for it
		if (disc[v] == INT_MIN)
		{
			SCCUtil(v, disc, low, st, stackMember);
			// Check if the subtree rooted with 'v' has a
			// connection to one of the ancestors of 'u'
			// Case 1 (per above discussion on Disc and Low value)
			low[u] = min(low[u], low[v]);
		}

		// Update low value of 'u' only of 'v' is still in stack
		else if (stackMember[v] == true)
			low[u] = min(low[u], disc[v]);
	}

	// head node found, pop the stack and print an SCC
	int w = 0;  // To store stack extracted vertices
	if (low[u] == disc[u])
	{
		while (st->top() != u)
		{
			w = (int)st->top();
			_strongComponent.push_back(w);
			stackMember[w] = false;
			st->pop();
		}
		w = (int)st->top();
		_strongComponent.push_back(w);
		stackMember[w] = false;
		st->pop();
	}
}

// The function to do DFS traversal. It uses SCCUtil()
std::unordered_map<int, std::vector<int>> GraphUtils::SCC()
{
	int *disc = new int[_vertex];
	int *low = new int[_vertex];
	bool *stackMember = new bool[_vertex];
	std::stack<int> *st = new std::stack<int>();

	// Initialize disc and low, and stackMember arrays
	for (int i = 0; i < _vertex; i++)
	{
		disc[i] = INT_MIN;
		low[i] = INT_MIN;
		stackMember[i] = false;
	}
	std::unordered_map<int, std::vector<int>> strongComponentMap;
	// Call the recursive helper function to find strongly
	// connected components in DFS tree with vertex 'i'
	for (int i = 0; i < _vertex; i++)
	{
		if (disc[i] == INT_MIN)
		{
			SCCUtil(i, disc, low, st, stackMember);
			if (_strongComponent.size() > 1)
			{
				strongComponentMap[i] = _strongComponent;
			}
		}
		_strongComponent.clear();
	}
	return strongComponentMap;
}



#ifdef TEST_GRAPH_UTILS

int main()
{
	DbCore<PayLoad> db;
	DbElement<PayLoad> elemdb;

	elemdb.name("DbCore.h.0");
	db.insert("DbCore.h.0", elemdb);

	GraphUtils gh;

	std::unordered_map<std::string, std::string> dbElem;
	dbElem[0] = 1;


	std::cout << "Create Graph";
	gh.createGraph(db, dbElem);


	std::cout << "Gets the created graph instance";
	auto ghInst = gh.getGraphInstance();

	std::unordered_map<int, std::vector<int>> unSCC = gh.SCC();
	std::cout << "Returns the cyclic components";
	/*
		std::cout << "Initialize check in";
		ch.intializeCheckIn();

		ch.updateClosedStatus();
		std::cout << "Updates the closed status";

		std::cout << "Adding dependencies to the checkin item";
		ch.addDependency({ "Test1" });*/
}

#endif TEST_GRAPH_UTILS
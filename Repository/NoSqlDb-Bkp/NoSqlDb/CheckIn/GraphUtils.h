#pragma once
/////////////////////////////////////////////////////////////////////////////
// GraphUtils.h - This package is responsible for creating graph using   //
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
* Graph.h, GraphUtils.h, GraphUtils.cpp, DbCore.h
*
* Maintenance History:
* ====================
* ver 1.0 : 10 Mar 2018
*			Initial Implementation
*/
#include "../GraphWalkDemo/Graph.h"
#include "../DbCore/DbCore.h"
#include "../DbCore/PayLoad.h"

using namespace NoSqlDb;

namespace LocalSVN
{
	/////////////////////////////////////////////////////////
	// This class is responsible for creating graphs and then 
	// for finding cyclic componenets in graph
	class GraphUtils
	{
	public:
		using Graph = Graph<std::string>;
		using Sptr = Sptr<std::string>;
		using Node = Node<std::string>;

		/////////////////////////////////////////////////////////
		// creates graph using db lemengts
		void createGraph(DbCore<PayLoad>& map, std::unordered_map<std::string, std::string> &idMap);

		/////////////////////////////////////////////////////////
		// Getter for graph instance
		Graph& getGraphInstance() { return _graph; }

		/////////////////////////////////////////////////////////
		// The function to do DFS traversal. It uses SCCUtil()
		std::unordered_map<int, std::vector<int>> SCC();    // collects strongly connected components

	private:
		/////////////////////////////////////////////////////////
		// Graph instance
		Graph _graph;
		/////////////////////////////////////////////////////////
		// // No. of vertices
		int _vertex;  
		/////////////////////////////////////////////////////////
		// A dynamic array of adjacency list
		std::unordered_map<int,std::list<int>> _adj;    
		/////////////////////////////////////////////////////////
		// contains strong components from Tarjan
		std::vector<int> _strongComponent;

		// A recursive function that finds and prints strongly connected
		// components using DFS traversal
		void SCCUtil(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[]);
	};
}
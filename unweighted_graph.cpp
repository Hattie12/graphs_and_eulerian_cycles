
#include "unweighted_graph.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <queue>
#include <iterator>
#include <vector>

using namespace std;

unweighted_graph::unweighted_graph(const string& t)
{
    //Code removed bc I didn't write this part
}

void unweighted_graph::display_adj_list() const{
    //Code removed bc I didn't write this part
}

//Inputs: none, Outputs: Whether or not the graph is connected, Details: determined via BFS
bool unweighted_graph::is_connected() const {

    //search queue with the first node added
    std::queue<int> node_q;
    node_q.push(0);

    //recored visited nodes
    std::vector<int> visited_nodes;
    //for checking whether the node was visited (bc I got a weird error using std::find)
    bool node_was_visited = false;

    //if there is nodes in the queue
    while (!node_q.empty()) {
        //extract the next node to check
        int node_to_check = node_q.front();
        node_q.pop();

        //see whether the node has already been visited
        node_was_visited = false;
        for_each(begin(visited_nodes), end(visited_nodes),
            [&node_was_visited, &node_to_check](int node) {if (node == node_to_check) { node_was_visited = true; }; });

        //if the node hasn't been visited
        if ( !node_was_visited ){

            //mark this node as visited
            visited_nodes.push_back(node_to_check);

            //add each node in the adj_list for this node
            for_each(begin(adj_list[node_to_check]), end(adj_list[node_to_check]),
                [&node_q](int node) {node_q.push(node); });

        }
    }

    //return whether all da nodes were reached by the BFS
    return (visited_nodes.size() == adj_list.size());
}

//Inputs: none, Outputs: whether or not the graph is eulerian, Details: true if all node degrees are even
bool unweighted_graph::is_eulerian() const {
    bool is_eulerian_flag = true;

    //changes flag if a node doesnt have an even degree
    auto evenDegree = [&is_eulerian_flag](const list<int> connected_nodes) {
        if ((connected_nodes.size() % 2)){
            is_eulerian_flag = false;
        } 
    };

    //see if each node has an even degree
    for_each(begin(adj_list), end(adj_list), evenDegree);

    return(is_eulerian_flag);
}

//Inputs: none, Outputs: A eulerian cycle in the form of a list
//Finds e. cycle via concatenating multiple loops without using repeated nodes
list<int> unweighted_graph::find_eulerian_cyc() const {
    
    list<int> cycle{0};
    list<int> cycle_to_append;
    int num_paths = 0;
    vector<list<int>> available_edges;

    //create the mask of available edges and count the number of paths
    for_each(begin(adj_list), end(adj_list),
        [&available_edges, &num_paths](list<int> edges) {
            list<int> binary_edges(edges.size(), 1);
            available_edges.push_back(binary_edges);
            num_paths += edges.size();
                                            });


    //while not all nodes are included in the cycle
    while (cycle.size() < num_paths/2){

        //find an additional cycle
        cycle_to_append = find_eulerian_cycle_helper(cycle, available_edges);

        //insert it by the node that the new cycle started and ended with
        cycle.insert(++find(begin(cycle), end(cycle), cycle_to_append.back()), begin(cycle_to_append),
            end(cycle_to_append));
    }

    return cycle;
}

//inputs: the eulerian cycle thus far, an adjacency list mask of available nodes where 1 = available
//outputs: a cycle that starts and ends at a node that is in the old_cycle and still has unused paths
//finds a cycle, not necessarily a eulerian one, using only unused paths
list<int> unweighted_graph::find_eulerian_cycle_helper(list<int> old_cyc, vector<list<int>>& available_nodes) const {

    //looks through the adj mask and determines if there is an available edge in it
    auto find_open_nodes = [](list<int> mask) {
        auto equals_one = [](int used) {return used; };

        if (find_if(begin(mask), end(mask), equals_one) != mask.end()) {
            return true;
        }
    };

    //find a node that is in the previous cycle and is linked via an unused path
    int start_node = distance(begin(available_nodes), find_if(begin(available_nodes), end(available_nodes), find_open_nodes));

    list<int> cycle;

    //the very first time the last node that needs sent to the find_new_edge function is the start_node
    cycle.push_back(find_new_edge(start_node, available_nodes));

    //create the cycle until it ends up where it began
    while (cycle.back() != start_node) {
        cycle.push_back(find_new_edge(cycle.back(), available_nodes));
    }

    return cycle;
}

//inputs: the node that begins the edge, an adjacency list mask of available nodes
//outputs: a node X where old_node -> X is a previously unused but existing edge
//method: found by finding the first availble edge in the available nodes mask and 
//correlating it to the actual node value
int unweighted_graph::find_new_edge(int last_node, vector<list<int>> & available_nodes) const {

    int next_node_index = 0; //will get bumped up while looking for available edge
    bool found_edge = false; //flag to note when to stop increasing index in lambda

    auto apply_mask = [&next_node_index, &found_edge](int unused_node) {

        //if an available edge has not been found
        if (!found_edge) {
            //and if the current node is available
            if (unused_node) {
                //leave the index at this element
                found_edge = true;
            }
            else {
                //increase the index
                next_node_index++;
            }
        }
    };

    //look through the edges for the last node and find one available
    for_each(begin(available_nodes[last_node]), end(available_nodes[last_node]), apply_mask);

    //pull out the next edge
    std::list<int>::const_iterator next_node_itr = cbegin(adj_list[last_node]);
    // Advance the iterator by 2 positions,
    std::advance(next_node_itr, next_node_index);

    //mark the edge as no longer availble
    //first mark the node as unavailable in the last_nodes's adjacency list
    std::list<int>::iterator unavailble_edge_itr = begin(available_nodes[last_node]);
    // Advance the iterator by 2 positions,
    std::advance(unavailble_edge_itr, next_node_index);
    *unavailble_edge_itr = 0;

    //then mark the node as unavailable in the next nodes's adjacency list
    //first, find where the last node is in the next node's adj list
    int last_node_index = 0; //will get bumped up while looking for last node
    bool found_last_node = false; //flag to note when to stop increasing index in lambda

    auto find_last_node_index = [&last_node_index, &found_last_node, last_node](int unused_node) {

        //if last node has not been found
        if (!found_last_node) {
            //and if the current node in adj list is last node
            if (unused_node == last_node) {
                //leave the index at this element
                found_last_node = true;
            }
            else {
                //increase the index
                last_node_index++;
            }
        }
    };

    for_each(begin(adj_list[*next_node_itr]), end(adj_list[*next_node_itr]), find_last_node_index);

    //then mark it as unavailable
    std::list<int>::iterator unavailble_edge_itr_2 = begin(available_nodes[*next_node_itr]);
    // Advance the iterator by 2 positions,
    std::advance(unavailble_edge_itr_2, last_node_index);
    *unavailble_edge_itr_2 = 0;

    return *next_node_itr;
}

//inputs: a cycle
//outputs: whether that cycle is a e. cycle
//method: makes sure the cycle is the same lenght as the list of edges and 
//hits all of the nodes, therefore checking that its an e. cycle
bool unweighted_graph::verify_eulerian_cyc(const list<int>& cycle) const {
    int num_paths = 0;

    //create the mask of available edges and count the number of paths
    for_each(begin(adj_list), end(adj_list),
        [&num_paths](list<int> edges) {
        num_paths += edges.size();
    });

    //the easy way out; cycle is the wrong length
    if (cycle.size() != num_paths / 2 + 1) {
        return false;
    }

    //create a list holding the value of all the nodes
    list<int> all_nodes(adj_list.size(), 0);
    int node_num = 0;
    for (int& node : all_nodes) {
        node += node_num++;
    }

    list<int> unique_found_nodes = cycle; //will hold the number of nodes reached by the cycle
    unique_found_nodes.sort();
    list<int>:: iterator end_it = unique(begin(unique_found_nodes), end(unique_found_nodes));

    //the cycle was the correct length but did not reach all the nodes
    if (distance(begin(unique_found_nodes), end_it) != node_num) {
        return false;
    }

    return true;
}


#ifndef unweighted_graph_H
#define unweighted_graph_H
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>


using namespace std;
class unweighted_graph
{
    public:
    unweighted_graph(const string& t);
    void display_adj_list() const;
    bool is_connected() const;
    bool is_eulerian() const;
    list<int> find_eulerian_cyc() const;
    bool verify_eulerian_cyc(const list<int>& cycle) const;
   
    private:
    vector<list<int>> adj_list;
    list<int> find_eulerian_cycle_helper(list<int> old_cyc, vector<list<int>> & available_nodes) const;
    int find_new_edge(int last_node, vector<list<int>>& available_nodes) const;
};
#endif

#ifndef LAYOUT_H
#define LAYOUT_H

#include <Windows.h>
#include "structures.h"
#include <vector>
#include <map>
#include <GL/glu.h>
#include "drawstreet.h"
#include "junction.h"
#include "buildingblocks.h"
#include "buildings.h"
#include "blocksubdivision.h"



class Layout {
public:
    Layout();
    void gridStraight(int start, int size, int increment);
    void gridCurved(int start, int size, int increment);
    void inputLayout(std::vector<std::vector<node>> nodePairs);
    void fileLayout(std::string filename);
    void populate();

    std::map<int, node> nodes;
    std::map<int, std::map<int, edge_axis>> edges;
    int count_node_id = 0;
    std::vector<std::vector<int>> objects = {};
    std::vector<std::vector<point>> objects_p = {};
    DrawStreet *street;
    BuildingBlocks *blocks;
    Junction *junction;
    Buildings *buildings;
    BlockSubdivision *subdivision;
    void updateMove(int move_node);
    void updateWidthSize(float width);

private:
    void initEdgeStorage();
    void addNode(node new_node);
    void addEdge(int node_s, int node_e, edge_axis edge);
    void updateScreen();
    float street_width = 1;
};

#endif // LAYOUT_H

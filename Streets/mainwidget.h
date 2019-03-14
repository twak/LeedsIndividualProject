#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QObject>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include "structures.h"
#include "DrawStreet.h"
#include "Layout.h"
#include "buildingblocks.h"
#include "junction.h"

using namespace std;

class MainWidget: public QGLWidget
    { Q_OBJECT//

    public slots:
        void cameraRotateHor(int);
        void cameraRotateVer(int);

    public:
        MainWidget(QWidget *parent);
        void OffsetLine(vector<float> s, vector<float> e);
        void DrawCurve(vector<float> p0, vector<float> p1, vector<float> p2, vector<float> p3);
        vector<float> Lerp(vector<float> p0, vector<float> p1, float t);
        void addEdge(int u, vector<int> v);
        void gridStreets(int start, int size, int increment);
        void radialStreets();
        void drawBlocks();
        void removeBlocks();
        void findBlocks_test();
        void DFS_test(int v, vector<vector<bool>> visited_edges, vector<int> found);
        float findIntersection(vector<float> s1, vector<float> e1, vector<float> s2, vector<float> e2);
        vector<vector<float>> calcOffsetLine(vector<float> s, vector<float> e);
        void addNode(vector<float> location);
        void addOffset(vector<int> edge_index, vector<vector<float>> offset);
        void initAdj();
        void initOffset();
        void findJunction(int node_index);
        float compareIntersection(vector<float> line1_s, vector<float> line1_e, vector<float> line2_s, vector<float> line2_e, float smallest);



    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

    private:
        int horVal = 0;
        int verVal = 0;
        vector<vector<vector<int>>> adj;
        vector<vector<float>> nodes = {};
        vector<vector<float>> support_nodes = {};
        vector<int> stack = {};
        vector<int> object_nodes = {};
        vector<vector<int>> objects = {};
        vector<vector<float>> edges = {};
        int count = 0;
        vector<vector<vector<vector<float>>>> offsetLines;
        DrawStreet* street = new DrawStreet();
        Layout *layout = new Layout(street);
        BuildingBlocks* blocks;
        Junction *junction;






}; // class


#endif // MAINWIDGET_H

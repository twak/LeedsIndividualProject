#include "drawstreet.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <string>

using namespace std;

DrawStreet::DrawStreet() {}

vector<point> DrawStreet::drawQuadCurvedLine(edge_axis street) {
    // returns a quadratic Bezier curve used for the turning radiuses
    point dot;
    float t = 0;
    vector<point> dots;

    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        dot.x = pow(1-t, 2)*street.start.x + 2*(1-t)*t*street.control[0].x + t*t*street.end.x;
        dot.y = 0;
        dot.z = pow(1-t, 2)*street.start.z + 2*(1-t)*t*street.control[0].z + t*t*street.end.z;
        dots.push_back(dot);
        t += 0.1;
    }
    return dots;
}

void DrawStreet::drawStraightLine(edge_axis street) {
    // draws a straight line (used only while debbbing)
    float t = 0;
    point dot;

    glBegin(GL_POINTS);
    glPointSize(1);
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        dot.x = (1-t)*street.start.x + t*street.end.x;
        dot.y = 0.1;
        dot.z = (1-t)*street.start.z + t*street.end.z;
        //t += street.step_size;
        t += 0.01;
        //glVertex3f(dot.x, dot.y, dot.z);
    }
    glEnd();
}

edge_axis DrawStreet::computeCurvedStreet(node start, node end, vector<point> control, float width) {
    // returns the dots of the offset curved street given start, end and control points
    float t = 0, line_length;
    point a, b, c, d, e, f, up_f, down_f;
    graphVector line_vector;
    vector<graphVector> normals;
    edge_axis street = computeStraightStreet(start, end, width);
    street.control = control;
    // find interpolation unti the deired limit is reached
    while ((1 - t) > -std::numeric_limits<double>::epsilon()) {
        a = Lerp(toPoint(street.start), street.control[0], t);
        b = Lerp(street.control[0], street.control[1], t);
        c = Lerp(street.control[1], toPoint(street.end), t);
        d = Lerp(a,b,t);
        e = Lerp(b,c,t);
        f = Lerp(d,e,t);

        line_vector = findVector(d, e);
        line_length = findLength(line_vector);
        normals = findNormals(line_vector, line_length);

        up_f = {f.x + normals[0].x*width, f.y + normals[0].y*width, f.z + normals[0].z*width};
        down_f = {f.x + normals[1].x*width, f.y + normals[1].y*width, f.z + normals[1].z*width};;

        t += 0.01;
        // assigned the new lines a offset ones
        street.offset_up.dots.push_back(up_f);
        street.offset_down.dots.push_back(down_f);
    }
    return street;
}

void DrawStreet::drawCurvedStreet(edge_axis street, vector<point> p_start, vector<point> p_end, int i_start, int i_end) {
    // draws a curved street using the previously stored dots
    point up_f, down_f;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    if (!p_start.empty()) {
        glVertex3f(p_start[0].x, p_start[0].y, p_start[0].z);
        glVertex3f(p_start[1].x, p_start[1].y, p_start[1].z);
    }
    for (int i = i_start; i <= i_end; i++) {
        up_f = street.offset_up.dots[i];
        down_f = street.offset_down.dots[i];
        glVertex3f(up_f.x, up_f.y, up_f.z);
        glVertex3f(down_f.x, down_f.y, down_f.z);
    }
    if (!p_end.empty()) {
        glVertex3f(p_end[0].x, p_end[0].y, p_end[0].z);
        glVertex3f(p_end[1].x, p_end[1].y, p_end[1].z);
    }
    glEnd();
}


point DrawStreet::Lerp(point p1, point p2, float t) {
    // does the linear line implementation
    point move = {(1-t)*p1.x + t*p2.x, (1-t)*p1.y + t*p2.y, (1-t)*p1.z + t*p2.z};
    return move;
}

edge_axis DrawStreet::computeStraightStreet(node start, node end, float width) {
    // offsets the lines of a straight street
    edge_axis street;
    graphVector line_vector = findVector(toPoint(start), toPoint(end));
    float line_length =  findLength(line_vector);
    vector<graphVector> normals = findNormals(line_vector, line_length);
    //float width = rand() % 100;
    //width = (width + 50) / 100;
    normals[0] = {normals[0].x*width, normals[0].y*width, normals[0].z*width};
    normals[1] = {normals[1].x*width, normals[1].y*width, normals[1].z*width};

    street.step_size = 1 / (line_length * 10);

    street.start = start;
    street.end = end;
    edge_offset offset_up, offset_down;
    offset_up.id = {start.id, end.id, 0};
    offset_down.id = {start.id, end.id, 1};
    offset_up.start = {start.x + normals[0].x, start.y + normals[0].y, start.z + normals[0].z};
    offset_up.end = {end.x + normals[0].x, end.y + normals[0].y, end.z + normals[0].z};
    offset_down.start = {start.x + normals[1].x, start.y + normals[1].y, start.z + normals[1].z};
    offset_down.end = {end.x + normals[1].x, end.y + normals[1].y, end.z + normals[1].z};
    offset_up.pair_p_intersection = {NAN,NAN,NAN};
    street.offset_up = offset_up;
    street.offset_down = offset_down;

    return street;
}

void DrawStreet::drawStraightStreet(edge_axis street, float t_start, float t_end) {
    // draws a straight street using the previously computed offset lines
    float t = t_start;
    edge_offset offset_up = street.offset_up;
    edge_offset offset_down = street.offset_down;
    point up, down;
    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,1,0);
    while ((t_end - t) > -std::numeric_limits<double>::epsilon()) {
        up = Lerp(offset_up.start, offset_up.end, t);
        down = Lerp(offset_down.start, offset_down.end, t);
        t += street.step_size;
        glVertex3f(up.x, up.y, up.z);
        glVertex3f(down.x, down.y, down.z);
    }
    glEnd();
}

graphVector DrawStreet::findVector(point s, point e) { graphVector v; v = {s.x - e.x, s.y - e.y, s.z - e.z}; return v;}

float DrawStreet::findLength(graphVector line_vector) { return sqrt(pow(line_vector.x,2) + pow(line_vector.y,2) + pow(line_vector.z,2)); }

vector<graphVector> DrawStreet::findNormals(graphVector line_vector, float line_length) {
    vector<graphVector> normals;
    normals.push_back({-line_vector.z/line_length, line_vector.y/line_length, line_vector.x/line_length});
    normals.push_back({line_vector.z/line_length, line_vector.y/line_length, -line_vector.x/line_length});
    return normals;
}


#include <fstream>
#include <iostream>
#include <sstream>
#include "util.h"

vector<vector<float> > getCorners (float x, float y, float z, float s) {
  return {
    {x - s, y - s, z - s},
    {x - s, y - s, z + s},
    {x - s, y + s, z - s},
    {x - s, y + s, z + s},
    {x + s, y - s, z - s},
    {x + s, y - s, z + s},
    {x + s, y + s, z - s},
    {x + s, y + s, z + s}
  };
}

vector<vector<vector<float> > > getTrianglesAndNormals (vector<vector<float>> corners) {
  return {
    {corners[0], corners[1], corners[2], {-1.0,  0.0,  0.0}},
    {corners[2], corners[1], corners[3], {-1.0,  0.0,  0.0}},
    {corners[1], corners[0], corners[4], { 0.0, -1.0,  0.0}},
    {corners[1], corners[4], corners[5], { 0.0, -1.0,  0.0}},
    {corners[0], corners[2], corners[4], { 0.0,  0.0, -1.0}},
    {corners[4], corners[2], corners[6], { 0.0,  0.0, -1.0}},
    {corners[2], corners[3], corners[6], { 0.0,  1.0,  0.0}},
    {corners[6], corners[3], corners[7], { 0.0,  1.0,  0.0}},
    {corners[5], corners[4], corners[6], { 1.0,  0.0,  0.0}},
    {corners[5], corners[6], corners[7], { 1.0,  0.0,  0.0}},
    {corners[3], corners[1], corners[5], { 0.0,  0.0,  1.0}},
    {corners[3], corners[5], corners[7], { 0.0,  0.0,  1.0}}
  };
}

string buildFacetString (vector<vector<float> > triangle) {
  stringstream s;
  vector<float> normal = triangle[3];
  s << "  facet normal " << normal[0] << "E+0 " << normal[1] << "E+0 " << normal[2] << "E+0" << endl;
  s << "    outer loop" << endl;
  for (int i = 0; i < 3; i++) {
    vector<float> point = triangle[i];
    s << "      vertex " << point[0] << "E+0 " << point[1] << "E+0 " << point[2] << "E+0" << endl;
  }
  s << "    endloop" << endl;
  s << "  endfacet" << endl;
  return s.str();
}

void exportStl (string filepath) {
  ofstream fs;
  fs.open(filepath + ".stl");
  if (!fs.is_open()) {
    cout << "Error: could not open file '" << filepath << ".stl'." << endl;
    return;
  }

  fs << "solid " << filepath << endl;

  //map<vector<int>, Voxel *> grid = world->getGrid();
  //for (const auto &pair : grid) {
      int size = 1;
      float x = static_cast<float>(1);
      float y = static_cast<float>(3);
      float z = static_cast<float>(0);
      float s = size / 2.0;

      vector<vector<float>> corners = getCorners(x, y, z, s);
      vector<vector<vector<float> > > triangles = getTrianglesAndNormals(corners);
      for (vector<vector<float> > t : triangles) {
        fs << buildFacetString(t);
      }
  //}

  fs << "endsolid " << filepath << endl;

  fs.close();
}

int main(int argc, char** argv){
  exportStl("test");
}

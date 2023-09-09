// A test file for te givio library
#include "GivIO.h"
#include <math.h>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
  giv::Contour circle;

  double xc=100, yc=100, r=50;

  for (int i=0; i<16; i++)
  {
    double th = 2*M_PI*i/16;
    double x = xc + r * cos(th);
    double y = yc + r * sin(th);
    circle.push_back({x,y});
  }

  giv::Giv giv;

  // Add a dataset to the giv file
  giv.push_back(
    {circle,
     {{"color", "red"},
      {"marks", "fcircle"},
      {"polygon", ""},
     },
     true // closed
    });
  ofstream fh("out.giv");
  giv.save(fh);
  fh.close();

  // Another example of a triangle
  giv.clear();
  giv::Giv giv1;
  vector<glm::dvec2> triangle = { { 0, 0}, {100,20}, { 30,80 } };
  giv.push_back(
    // A dataset
    {
      triangle,                     // dataset points forming a path
      {                             // dataset attributes
        {"color", "red/.2"},         
        {"outline_color", "black"},
        {"lw", "5"},
        {"balloon", "A triangle" },
        {"polygon", ""}},
      true // the path is closed 
    });
  giv.save("triangle.giv");

  giv::Giv giv2("triangle.giv");
  giv2[0].attribs()["color"] = "green/.2";
  giv2.save("green-triangle.giv");
  
  exit(0);
}

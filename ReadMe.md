# Intro

Giv is an image and vector viewer available at http://github.com/dov/giv. This library contains code for reading and parsing giv files in C++.

# Contents

For the python library see python. The main directory contains the c++ code.

# Example

```C++

  // Create a giv dataset and save it to disk
  giv::Giv giv1;
  vector<glm::dvec2> triangle = { { 0, 0}, {100,20}, { 30,80 } };
  giv1.push_back(
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
  giv1.save("triangle.giv");
  
  // read a dataset and modify it
  giv::Giv giv2("triangle.giv");
  giv2[0].attribs()["color"] = "green/.2";
  giv2.save("green-triangle.giv");
  

```

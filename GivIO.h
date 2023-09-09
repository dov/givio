//======================================================================
//  GivIO.h - An interface to reading and writing giv files
//
//   - Add builder for giv files.
//
//  Dov Grobgeld <dov.grobgeld@gmail.com>
//  Mon Jan  3 10:13:52 2022
//----------------------------------------------------------------------
#ifndef GIVIO_H
#define GIVIO_H

#include <map>
#include <vector>
#include <exception>
#include <glm/vec2.hpp>
#include <stdexcept>

namespace giv
{
  
class giv_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

enum class Op { OP_MOVE_TO, OP_LINE_TO, OP_CLOSEPATH };

// A pathpoint is composed of glm::dvec and an path operator
class PathPoint : public glm::dvec2
{
 public:
  // Constructors
  PathPoint(Op Op=Op::OP_MOVE_TO, const glm::dvec2& v={0,0}):
    glm::dvec2(v), m_op(Op) {}

  PathPoint(const glm::dvec2& v):
    glm::dvec2(v), m_op(Op::OP_LINE_TO) {}

  Op m_op;
};

using PathPoints = std::vector<PathPoint>;
using Attribs = std::map<std::string, std::string>;
using Contour = std::vector<glm::dvec2>;
using Contours = std::vector<Contour>;

// The DataSet class. A dataset is composed of path points and
// attributes.
class DataSet : public PathPoints {
  public:
    // Constructor from a path points. This gives full control
    // of whether to move to or line to between each path points.
    DataSet(const PathPoints& points = PathPoints(),
             Attribs attribs = Attribs())
      : PathPoints(points),
        m_attribs(attribs)
    {}
  
    // Constructor for importing a simplified Contour structure
    DataSet(const Contour& contour,
             const Attribs& attribs=Attribs(),
             bool closed=false);
  
    // Constructor for importing a simplified Contours structure.
    // Each contour starts causes a new move to.
    //
    // closed determines whether contained contour is closed.
    DataSet(const Contours& contour,
             const Attribs& attribs=Attribs(),
             bool closed=false);
  
    // Save the dataset to the filehandle. Optionally amending the
    // attribs.
    void save(std::ofstream& fh, Attribs attribs = Attribs()) const;
  
    // Set the attributes from the given attribute list. If Merge is true
    // then the attributes will be merged with the current attributes
    // otherwise the attributes are replaced.
    void set_attribs(const Attribs& attribs, bool merge=false);

    // Get a reference to the attribs (which can be modified)
    Attribs& attribs(void) { return m_attribs; };

  private:
    Attribs m_attribs;
};

// A parser and container for giv files and data
class Giv : public std::vector<DataSet>
{
  public:
    Giv(const std::string& Filename="");
    void parse_giv_file(const std::string& Filename);
  
    // Return a dataset as a contour (assume closed)
    Contour get_as_contour(int DataSetId = 0);
  
    // Save all the datasets
    void save(std::ofstream& fh) const;
  
    // Save the all the datasets in the given file.
    void save(const std::string& Filename) const;
  
    // Create a joined dataset of all the datasets in the object.
    // The attributes will be merged by the last value set.
    DataSet join();
};
};

#endif /* GIVIO */


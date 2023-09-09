//======================================================================
//  GivIO.cpp - A class for reading and writing giv files in modern
//  C++.
//
//  Dov Grobgeld <dov.grobgeld@gmail.com>
//  2023-09-09 Sat
//----------------------------------------------------------------------

#include <string_view>
#include <vector>
#include "GivIO.h"
#include <fstream>
#include <fmt/core.h>
#include <charconv>

using namespace std;
using namespace giv;

// Constructor for importing a Contours structure
DataSet::DataSet(const Contours& Paths, const Attribs& attribs, bool Closed)
{
  for (auto& Path : Paths)
  {
    bool first = true;
    for (auto& p : Path)
    {
      Op op = first ? Op::OP_MOVE_TO : Op::OP_LINE_TO;
      if (first)
        first = false;
        
      this->push_back({op,{p.x,p.y}});
    }
    if (Closed)
      this->push_back({Op::OP_CLOSEPATH,{0,0}});
  }

  set_attribs(attribs);
}

// Constructor for importing a Contour structure
DataSet::DataSet(const Contour& Path, const Attribs&  attribs, bool Closed)
{
  bool first = true;
  for (auto& p : Path)
  {
    Op op = first ? Op::OP_MOVE_TO : Op::OP_LINE_TO;
    if (first)
      first = false;
      
    this->push_back({op,{p.x,p.y}});
  }
  if (Closed)
    this->push_back({Op::OP_CLOSEPATH,{0,0}});

  set_attribs(attribs);
}

// Split a string ignoring leading and trailing and multiple spaces
static vector<std::string_view> white_space_split(const std::string_view Subject)
{   
  std::vector<std::string_view> Res;

  int LeadingSpace = 0;
  while(LeadingSpace < (int)Subject.size() &&
        (Subject[LeadingSpace]==' ' || Subject[LeadingSpace]=='\r'))
    LeadingSpace++;

  int PosRight = LeadingSpace;

  for (int i=LeadingSpace;i<(int)Subject.size();i++)
  {
    if (Subject[i] == ' ' || Subject[i] == '\r')
    {
      int PosLeft = PosRight;
      PosRight = i;

      Res.push_back({Subject.data() + PosLeft, size_t(PosRight - PosLeft)});

      // skip additional whitespace
      while(i < (int)Subject.size() && (Subject[i] == ' ' || Subject[i] == '\r'))
        i++;
      PosRight = i;
    }
  }

  // Only add last word if it is non-white space
  bool GotNonWhiteSpace = false;
  for (size_t i=PosRight; i<Subject.size(); i++)
    if (Subject[i]!=' ')
    {
      GotNonWhiteSpace=true;
      break;
    }
  if (GotNonWhiteSpace)
    Res.push_back({ Subject.data() + PosRight, Subject.size() - PosRight});

  return Res;
}

static double to_double(const std::string_view& View)
{
  double d;
  from_chars(View.data(), View.data() + View.size(), d);
  return d;
}

void DataSet::set_attribs(const Attribs& attribs, bool Merge)
{
  if (!Merge)
    m_attribs.clear();

  for (auto& [k,v] : attribs)
    m_attribs[k] = v;
}

void DataSet::save(ofstream& fh, Attribs attribs) const
{
  // Merge the given attribs with the attribs from the  dataset
  auto new_attribs = m_attribs;
  for (auto &[k,v] : attribs)
    new_attribs[k] = v;
  
  for (auto &[k,v] : new_attribs)
    fh << fmt::format("${} {}\n", k,v);

  for (auto& p : *this)
  {
    if (p.m_op == Op::OP_MOVE_TO)
      fh << "m ";
    else if (p.m_op == Op::OP_CLOSEPATH)
    {
      fh << "z\n";
      continue;
    }

    fh << fmt::format("{} {}\n", p.x, p.y);
  }
  fh << "\n";
}

// Constructor
Giv::Giv(const string& Filename)
{
  if (Filename.size())
    parse_giv_file(Filename);
}

void Giv::parse_giv_file(const string& Filename)
{
  ifstream fh(Filename);

  this->clear();
  
  if (!fh.good())
    throw giv_error(fmt::format("Failed opening file: {}", Filename));
  string line;
  Attribs attribs;
  PathPoints points;

  while(!fh.eof())
  {
    getline(fh, line);

    auto args = white_space_split(line);
    if (args.size() == 0) // empty line
    {
      if (points.size())
      {
        push_back({points, attribs});
        points.clear();
        // Attribs are sticky
      }
      continue;
    }
    if (args.size()>1 && args[0][0]=='$')
    {
      // Is there a better way of getting everything till the end of the line
      attribs.insert({string(args[0]).substr(1),
          line.substr(args[1].begin()-args[0].begin())});
      continue;
    }
    else if (args.size()==1 && args[0][0]=='$')
      attribs.insert({string(args[0]).substr(1), ""});

    char ch = tolower(args[0][0]);
    if (ch == 'm')
      points.push_back({Op::OP_MOVE_TO, {to_double(args[1]), to_double(args[2])}});
    else if (ch == 'z')
      points.push_back(Op::OP_CLOSEPATH);
    else if (ch == 'l')
      points.push_back({Op::OP_LINE_TO, {to_double(args[1]), to_double(args[2])}});
    else if (isdigit(ch) || ch=='-' || ch=='.')
      points.push_back({Op::OP_LINE_TO, {to_double(args[0]), to_double(args[1])}});
    // otherwise ignore
      
  }
  if (points.size())
    push_back({points, attribs});

  fh.close();
}

Contour Giv::get_as_contour(int data_set_id)
{
  Contour contour;

  if (data_set_id >= (int)this->size())
    throw giv_error("No such Dataset!");

  for (auto& p : (*this)[data_set_id])
    if (p.m_op!= Op::OP_CLOSEPATH)
      contour.emplace_back(p.x, p.y);
  return contour;
}

// Save all the datasets
void Giv::save(std::ofstream& ofh) const
{
  for (auto& ds : *this)
    ds.save(ofh);
}

// Overloaded to create a file
void Giv::save(const string& Filename) const
{
  ofstream ofh(Filename);
  if (!ofh.good())
    throw giv_error(fmt::format("Failed opening {} for writing!", Filename));

  save(ofh);
  ofh.close();
}

// Join all paths to a dataset. The attributes will be merged and overwritten
DataSet Giv::join()
{
  DataSet joined_data_set;

  Attribs attribs;
  for (auto& ds : *this)
  {
    bool First=true;
    for (auto p : ds)
    {
      if (First)
        p.m_op = Op::OP_MOVE_TO;
      First=false;
      joined_data_set.push_back(p);
    }
    // Averride the attributes
    for (auto& [k,v] : ds.attribs())
      attribs[k] = v;
  }
  joined_data_set.set_attribs(attribs);

  return joined_data_set;
}

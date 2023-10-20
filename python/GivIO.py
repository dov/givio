#!/usr/bin/python

######################################################################
#  A Giv IO module in python
#
#  There are lots of possible options for how to efficiently
#  store the giv coordinates for a dataset. This module stores
#  them in a pandas Dataframe.
#
#  The native giv coordinate type is a point which contains (op,x,y),
#  where op is one of LINETO, MOVETO, CLOSEPATH.
#
#  But there are method for inputting exporting "coords" that
#  are don't have an op-code.
#
#  License:
#     This file is licensed under the GNU LESSER GENERAL
#     PUBLIC LICENSE v2.0
#   
#  Dov Grobgeld 
#  2023-09-10 Sun
######################################################################

import copy
import numpy as np
import pandas as pd
import re

# A point in giv is a line in a pandas datastructure containing the op code
#
OP_MOVE_TO=0
OP_LINE_TO=1
OP_CLOSEPATH=2
    
# A point array is encoded as the following pandas dataframe:
#
#    columns=('op','x','y')
#    dtypes=('i8','f8','f8')
#
# And a coord array is encoded as the following pandas dataframe
#
#    columns = ('x','y')
#    dtypes = ('f8','f8')
#

def coords_to_points(coords, is_closed=True):
    '''Convert a coords dataframe into a points dataframe'''
    if not isinstance(coords, np.ndarray):
        coords = np.array(coords)

    # Manipulate as an ndarray and build the points
    n = coords.shape[0]
    if is_closed:
        n+=1

    points = np.zeros((n,3))
    points[n-1, 0] = OP_MOVE_TO
    points[:coords.shape[0],1:] = coords
    points[1:coords.shape[0],0] = OP_LINE_TO
    if is_closed:
        points[n-1,0] = OP_CLOSEPATH

    # And convert it to a dataframe
    df= pd.DataFrame(points, columns=['op','x','y'])
    df['op'] = df['op'].astype(np.int64)
    return df

def points_to_coords(points):
    '''Currently ignore op commands coords, but if closepath repeat the first coord'''
    return points[['x','y']].copy()

def validate_points(points):
    '''If points is a list of tuples, turn it into a dataframe'''
    if not isinstance(points, pd.DataFrame):
        points = pd.DataFrame(points, columns=['op','x','y'])
        points['op'] = points['op'].astype(np.int64)
    return points

class DataSet:
    '''A DataSet contains a list of points and their attributes'''
    def __init__(self, points=None, attribs=None, coords = None, is_closed=True):
        '''Coords allows setting coordinates without the moveto lineto info'''
        self.attribs = {}
        self.points = points
        if coords is not None:
            self.points = coords_to_points(coords,is_closed=is_closed)
        self.points = validate_points(self.points) # Upgrade from tuples
        self.attribs = attribs
  
    def __getitem__(self,index):
        return self.points[index]
  
    def __len__(self):
        return len(self.points)
  
    def save_to_fh(self, fh):
        if self.attribs is not None:
            for k,v in self.attribs.items():
                fh.write(f'${k} {v}\n')
        # This can probably be made faster
        op_string = ['m','','z']
        for _,(op,x,y) in self.points.iterrows():
            if op==OP_CLOSEPATH:
                fh.write('z\n')
            else:
                fh.write(f'{op_string[int(op)]} {x} {y}\n')
        fh.write('\n\n')
  
    def copy(self):
        other = DataSet(points = self.points.copy(),
                        attribs = copy.deepcopy(self.attribs))
  
    def set_attribs(self, attribs):
        self.attribs = attribs
  
    def set_points(self, points):
        self.points = points

    def get_coords(self):
        return points_to_coords(self.points)

    def get_points(self):
        return self.points

class Giv:
    '''A Giv class is conceptually a list of datasets'''
    def __init__(self, filename=None):
        self.datasets = []
        if filename:
            self.parse_file(filename)
    
    def __getitem__(self,index):
        return self.datasets[index]
  
    def __len__(self):
        return len(self.datasets)
  
    def show(self):
        # TBD - use tempfile                        
        Filename = f'/tmp/givio{os.getpid()}.giv' 
        with open(Filename,'w') as fh:
            self.save_to_fh(fh)
        os.system(f'giv {Filename}')
  
    def add_dataset(self, dataset):
        self.datasets.append(dataset)
  
    def parse_file(self, filename):
        attribs = {}
        points = []
        with open(filename) as fh:
            for line in fh:
              if line.startswith('$'):
                  line = line[1:-1]
                  kv = line.split(' ',maxsplit=1)
                  if len(kv)==1:
                      key = kv[0]
                      val = ''
                  else:
                      key,val = kv
                  attribs[key]=val
                  continue
              if re.search(r'^\s*$', line):
                  if len(points):
                      self.add_dataset(
                        DataSet(points=points, attribs=copy.deepcopy(Attribs)))
                      # Attribs are sticky
                      Points = []
                  continue
              vals = line.split()
              op = vals[0][0].lower()
              if op == 'm':
                 points.append((OP_MOVE_TO, float(vals[1]), float(vals[2])))
              elif op == 'l':
                 points.append((OP_LINE_TO, float(vals[1]), float(vals[2])))
              elif op == 'z':
                  points.append((OP_CLOSEPATH, 0,0))
              elif len(vals)==2:
                  points.append(((OP_MOVE_TO if len(points)==0 else OP_LINE_TO),
                                float(vals[0]), float(vals[1])))
                          
        if len(points):
            self.add_dataset(DataSet(points=points,
                                     attribs=attribs))
  
    def save_to_fh(self, fh):
        for ds in self.datasets:
            ds.save_to_fh(fh)
            fh.write('\n')
  
    def save(self, filename, append=False):
        mode = 'a' if append else 'w'
        with open(filename,mode) as fh:
            self.save_to_fh(fh)
        

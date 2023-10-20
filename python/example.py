######################################################################
#  An example of how to use GivIO in python.
#
#  2023-10-20 Fri
#  Dov Grobgeld <dov.grobgeld@gmail.com>
######################################################################

import pandas as pd
from GivIO import Giv, DataSet

giv = Giv('rect.giv')

# Modify the attributes of the first dataset
giv[0].attribs['color'] = 'green'
giv[0].attribs['balloon'] = 'I am green'

# Add a new dataset
giv.add_dataset(
    DataSet(coords = [(50,50),(100,50),(100,100)],
            attribs= {
                'color':'blue/0.3',
                'polygon':'',
                'outline_color':'blue',
                'lw' : 5,
                'balloon' : 'I am <span color="blue" weight="bold">BLUE</span>'
            },
            is_closed=True))
                        
# Print the points of our new dataset
print(giv[1].get_points())

# Save the result
giv.save('rect-copy.giv')

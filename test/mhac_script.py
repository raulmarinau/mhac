import sys
sys.path.append("/home/marin/projects/mhac/build")
import mhac

import random, copy, math
import matplotlib.pyplot as plt
import numpy as np

def read_TSP_data(filepath="../data/tsp/eil101.tsp"):
    cities = mhac.physics.Cities()

    city = mhac.physics.City(1, 1)
    cities.append(city)

    # with open(filepath, 'r') as infile:
    #     Name = infile.readline().strip().split()[1] # NAME
    #     FileType = infile.readline().strip().split()[1] # TYPE
    #     Comment = infile.readline().strip().split()[1] # COMMENT
    #     Dimension = infile.readline().strip().split(":")[1] # DIMENSION
    #     EdgeWeightType = infile.readline().strip().split()[1] # EDGE_WEIGHT_TYPE
    #     infile.readline()

    #     for i in range(int(Dimension)):
    #         coords = infile.readline().strip().split()[1:]
    #         cities.append(mhac.physics.City(int(coords[0]), int(coords[1])))

    return cities

# def displayTour(cities, solution):
#     plt.figure(figsize = (16,8))
#     N = len(cities)

#     plt.axes()
#     plt.plot([cities[solution[i % N]].x for i in range(N+1)], [cities[solution[i % N]].y for i in range(N+1)], 'bo-')
#     plt.show()

read_TSP_data()
# problem = mhac.physics.TSP(read_TSP_data())
# SA = mhac.physics.SimulatedAnnealing(problem)
# SA.solve(1000., 0.000001)

# displayTour(problem.mCities, SA.getSolution())
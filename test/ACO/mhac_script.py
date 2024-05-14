import sys
sys.path.append("/home/marin/projects/mhac/build/debug/")
import mhac

import random, copy, math
import matplotlib.pyplot as plt
import numpy as np

def read_TSP_data(filepath="../../data/tsp/eil101.tsp"):
    cities = mhac.problems.tsp.Cities()

    with open(filepath, 'r') as infile:
        Name = infile.readline().strip().split()[1]
        FileType = infile.readline().strip().split()[1]
        Comment = infile.readline().strip().split()[1]
        Dimension = infile.readline().strip().split(":")[1]
        EdgeWeightType = infile.readline().strip().split()[1]
        infile.readline()

        for i in range(int(Dimension)):
            coords = infile.readline().strip().split()[1:]
            city = mhac.problems.tsp.City(int(coords[0]), int(coords[1]))
            cities.append(city)

    return cities

def displayTour(cities, solution):
    plt.figure(figsize = (16,8))
    N = len(cities)

    plt.axes()
    plt.plot([cities[solution[i % N]].x for i in range(N+1)], [cities[solution[i % N]].y for i in range(N+1)], 'bo-')
    plt.show()

problem = mhac.problems.tsp.ACO_TSP(read_TSP_data())
ACO = mhac.swarm.AntColonyOptimization(problem)
sol = ACO.solve(150, 20, 0.5, 0.5, 0.25)
displayTour(problem.mCities, sol.tour)
sol.cost
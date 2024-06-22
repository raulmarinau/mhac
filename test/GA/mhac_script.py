import sys
sys.path.append("/home/marin/projects/mhac/build/debug/")
import mhac

import random, copy, math, os, time
import numpy as np
from queue import Queue

folder_path = "../../data/jss/imrg/ds1/testbed_1_s"
best_path = "../../data/jss/imrg/ds1/best/testbed1_small.csv"
output_file = "results/mhac.csv"

def read_file_input(filepath):
    with open(filepath) as f:
        lines = f.readlines()
        sizes = [int(nr) for nr in lines[0].split()]
        m, n = sizes[0], sizes[1]
        arr = np.zeros((n, m))

        for i in range(1, len(lines)):
            arr[i-1, :] = [int(nr) for nr in lines[i].split()]

    return arr

def nparray_to_timematrix(arr):
    tm = mhac.problems.jss.TimeMatrix()

    for time_list in arr.tolist():
        vint = mhac.VectorInt()
        for duration in time_list:
            vint.append(int(duration))
        tm.append(vint)

    return tm

def read_all_files_in_folder(folder_path):
    # List to hold file contents
    file_contents = {}

    # Check if the provided path is a directory
    if not os.path.isdir(folder_path):
        raise ValueError(f"The provided path '{folder_path}' is not a valid directory.")

    # Iterate over all files in the directory
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)

        # Check if it's a file
        if os.path.isfile(file_path):
            try:
                # Open and read the file
                with open(file_path, 'r') as file:
                    content = file.read()
                    file_contents[filename] = content
            except Exception as e:
                print(f"Error reading file {filename}: {e}")
    
    return file_contents

class PythonJSSP(mhac.common.Problem):
    def __init__(self, processing_times):
        super().__init__()
        self.processing_times = processing_times
        self.N = len(processing_times)
        self.M = len(processing_times[0])

    def generateInitialSolution(self):
        sol = mhac.problems.jss.JSSS()
        sol.schedule = random.sample(range(self.N), self.N)
        return sol

    def generateNewSolution(self, initialSol: mhac.problems.jss.JSSS):
        i, j = sorted(random.sample(range(self.N), 2))
        newSol = mhac.problems.jss.JSSS()
        newSol.schedule = initialSol.schedule  # copying with pybind, not using python refs
        # old_segment = newSol.tour[i:j+1]
        # newSol.tour[i:j+1] = old_segment[::-1]  # Slice and reverse the segment
        newSol.schedule[i], newSol.schedule[j] = newSol.schedule[j], newSol.schedule[i]
        return newSol

    def evaluateSolution(self, sol: mhac.problems.jss.JSSS):        
        # Initialize completion times matrix
        completion_times = [[0] * self.M for _ in range(self.N)]
        total_completion_time = 0

        for job_index in range(self.N):
            job = sol.schedule[job_index]

            for machine in range(self.M):
                if job_index == 0:
                    if machine == 0:
                        completion_times[job_index][machine] = self.processing_times[job][machine]
                    else:
                        completion_times[job_index][machine] = completion_times[job_index][machine-1] + self.processing_times[job][machine]
                else:
                    if machine == 0:
                        completion_times[job_index][machine] = completion_times[job_index-1][machine] + self.processing_times[job][machine]
                    else:
                        completion_times[job_index][machine] = self.processing_times[job][machine] + max(completion_times[job_index][machine-1], completion_times[job_index-1][machine])

                if machine == self.M - 1:
                    total_completion_time += completion_times[job_index][machine]

        return total_completion_time
    
class PythonJSSP_ACO(mhac.swarm.Problem, PythonJSSP):
    def __init__(self, processing_times):
        mhac.swarm.Problem.__init__(self)
        PythonJSSP.__init__(self, processing_times)

    def updateAntPath(self, ant, pm, alpha, beta):
        schedule_size = len(ant.schedule)
        probabilities = np.zeros(schedule_size)
        
        for i in range(schedule_size-1):
            current_job = ant.schedule[i]
            sum_probabilities = 0.0

            for j in range(i+1, schedule_size):
                next_job = ant.schedule[j]
                # Use the correct method to access elements from PheromoneMatrix
                pheromone = pm(current_job, next_job) ** alpha
                
                # Calculate the heuristic value
                heuristic = 0 if (self.processing_times[current_job][next_job] == 0) else 1.0 / self.processing_times[current_job][next_job]
                eta = heuristic ** beta
                probabilities[j] = pheromone * eta

                sum_probabilities += probabilities[j]

            if sum_probabilities > 0:
                probabilities[i+1:schedule_size] /= sum_probabilities
                selected_index = random.choices(range(i + 1, schedule_size), weights=probabilities[i + 1:schedule_size])[0]
                ant.schedule[i + 1], ant.schedule[selected_index] = ant.schedule[selected_index], ant.schedule[i + 1]

        return ant

    def updatePheromoneMatrix(self, ant, pm, rho):
        total_job_completion_time = self.evaluateSolution(ant)
        deposit = 1.0 / total_job_completion_time

        # Evaporate the existing pheromone
        size = pm.getSize()
        for i in range(size):
            for j in range(size):
                current_value = pm(i, j)  # Get current value
                pm.set(i, j, current_value * (1 - rho))  # Set new value

        # Deposit new pheromones based on the ant's path
        for k in range(len(ant.schedule) - 1):
            i = ant.schedule[k]
            j = ant.schedule[k + 1]
            pm.set(i, j, pm(i, j) + deposit)  # Update the value


def process_files_pyjssp(file_data):
    results = []
    total_time = 0
    for filename, content in file_data.items():
        # problem = PythonJSSP(nparray_to_timematrix(read_file_input(f"{folder_path}/{filename}")))
        # SA = mhac.physics.SimulatedAnnealing(problem)

        # problem = PythonJSSP_GA(nparray_to_timematrix(read_file_input(f"{folder_path}/{filename}")))
        # GA = mhac.evolutionary.GeneticAlgorithm(problem)

        problem = PythonJSSP_ACO(nparray_to_timematrix(read_file_input(f"{folder_path}/{filename}")))
        ACO = mhac.swarm.AntColonyOptimization(problem)

        start_time = time.time()

        # sol = SA.solve(maxT=100, minT=0.1, k=0.995)
        # sol = GA.solve(generations=100, populationSize=20, mutationChance=0.75, selectionSize=5, selectionType=mhac.evolutionary.SelectionType.TOURNAMENT)
        sol = ACO.solve(generations=1000, colonySize=20, alpha=0.55, beta=1.0, rho=0.15)
        
        end_time = time.time()
        duration = end_time - start_time

        print(f"Processing file: {filename} took {duration:.4f}s")
        print(f"Schedule: {sol.schedule} Cost: {sol.cost}")

        results.append({"filename": filename, "cost": sol.cost, "time": duration})
        total_time += duration

    print(f"Total time spent solving: {total_time:.4f}s")
    return results

file_data = read_all_files_in_folder(folder_path)
results = process_files_pyjssp(file_data)

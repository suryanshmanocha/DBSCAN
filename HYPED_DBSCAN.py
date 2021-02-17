import matplotlib.pyplot as plt
import math
import random


"""
DBSCAN implementation in Python
The design choices of this implementation were made for portability into C++
Note: More optimal, python-specific design choices could be made.
"""


class DBSCAN:

    DATA_TYPES = ("Unknown", "Noise", "Border", "Core")
    DATA = []
    DATA_X = []
    VISIT_ORDER = []  # List of indexes of len(DATA), randomly shuffled
    current_idx = -1  # Current index for VISIT_ORDER

    CORE = set()
    BORDER = set()
    NOISE = set()

    def __init__(self, epsilon, min_points, data):
        self.epsilon = epsilon
        self.min_points = min_points

        self._parse_data(data)
        self._run()

    def _run(self):
        for _ in range(len(self.DATA)):
            self._init_rand()
            while self.current_idx != len(self.DATA)-1:
                self._logic_handler()

    """
    Handles main logic of DBSCAN class
    """
    def _logic_handler(self):
        # Step 1
        data_point = self._visit_random()
        # If we already know data is a core point, skip
        if data_point.type == 3:
            return
        # Step 2
        neighbours = self._init_neighbours(data_point)
        if not self._has_min_vertical_neighbours(data_point, neighbours):
            data_point.type = 1
            self.NOISE.add(data_point)
            return
        if len(neighbours) >= self.min_points:
            # Start a cluster
            data_point.type = 3
            self.CORE.add(data_point)
        else:
            # Label xi as noise for now
            for neighbour in neighbours:
                if neighbour.type == 3:
                    # If core point as neighbour, then border point
                    data_point.type = 2
                    self.BORDER.add(data_point)
            data_point.type = 1
            self.NOISE.add(data_point)

    """
    If there are no vertical neighbours, then consider outlier 
    """
    @staticmethod
    def _has_min_vertical_neighbours(data_point, neighbours):
        for neighbour in neighbours:
            if neighbour.xval == data_point.xval:
                return True
        return False

    """
    Visit random data point in the data set
    """
    def _visit_random(self):
        self.current_idx += 1
        return self.DATA[self.VISIT_ORDER[self.current_idx]]

    def _init_neighbours(self, data_point):
        neighbours = []

        for datum in self.DATA:
            if datum.val != data_point.val:
                if self._euclidean_dist(datum, data_point) <= self.epsilon:
                    neighbours.append(datum)
                    datum.add_neighbour(data_point)
                    data_point.add_neighbour(datum)

        return neighbours

    @staticmethod
    def _euclidean_dist(p, q):
        dist = math.sqrt(
            sum((
                (p.val - q.val)**2,
                (p.xval - q.xval)**2,
            ))
        )
        return dist

    """
    Parse raw data input: Assuming data input is array of arrays
    """
    def _parse_data(self, data):
        for i in range(len(data[0])):
            for j in range(len(data[0][i])):
                self.DATA.append(DataPoint(data[0][i][j], data[1][i][j]))

        self.DATA_X = data[1]

    def _init_rand(self):
        self.VISIT_ORDER = [i for i in range(len(self.DATA))]
        random.shuffle(self.VISIT_ORDER)
        self.current_idx = -1

    def get_outliers(self):
        return self.NOISE

    def get_core(self):
        return self.CORE

    def get_border(self):
        return self.BORDER


class DataPoint:

    def __init__(self, val, xval):
        self.val = val
        self.xval = xval
        self.neighbours = []
        self.type = 0

    def add_neighbour(self, neighbour):
        self.neighbours.append(neighbour)


def data_reader(epsilon, restrict_size=20):
    with open("squashed_data.txt") as file:
        raw_data_arr = [line.split(' ') for line in file.readlines()]
        data_arr = []

        for arr in raw_data_arr:
            prcsd_data = []
            for i in range(1, 5):
                data = arr[i]
                data.strip('\n')
                prcsd_data.append(float(data))
            data_arr.append(prcsd_data)

    data_arr_x = [[i * epsilon*0.9] * 4 for i in range(len(data_arr))]

    return data_arr[:restrict_size], data_arr_x[:restrict_size]


def run():
    data_size = 30
    epsilon = 0.18/2  # Half of the average variance?
    min_points = 2

    dbscan = DBSCAN(epsilon, min_points, data_reader(epsilon, data_size))
    outliers = [c.val for c in dbscan.get_outliers()]
    core = [c.val for c in dbscan.get_core()]
    border = [c.val for c in dbscan.get_border()]
    print("Core: {}".format(core))
    print("Border: {}".format(border))
    print("Outliers: {}".format(outliers))

    ## PLOTTING STUFF ##
    data_arr, data_arr_x = data_reader(epsilon, data_size)

    plt.scatter(data_arr_x, data_arr)
    [plt.scatter(c.xval, c.val, c='b') for c in dbscan.get_core()]
    [plt.scatter(c.xval, c.val, c='y') for c in dbscan.get_border()]
    [plt.scatter(c.xval, c.val, c='r') for c in dbscan.get_outliers()]
    plt.show()


run()


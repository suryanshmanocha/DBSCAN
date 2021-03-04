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
    DATA = []  # List of DataPoint objects, in time series order
    VISIT_ORDER = []  # List of indexes of len(DATA), randomly shuffled
    current_idx = -1  # Current index for VISIT_ORDER

    CORE = set()
    BORDER = set()
    NOISE = set()

    def __init__(self, epsilon, min_points, data, num_imus=4):
        self.epsilon = epsilon
        self.min_points = min_points
        self.num_imus = num_imus

        self._parse_data(data)
        self._run()

    """
    Parse raw data input: Assuming data input is array of arrays
    """
    def _parse_data(self, data):
        for i in range(len(data[0])):
            for j in range(len(data[0][i])):
                self.DATA.append(DataPoint(data[0][i][j], data[1][i][j]))

    def _init_rand(self):
        self.VISIT_ORDER = [i for i in range(len(self.DATA))]
        random.shuffle(self.VISIT_ORDER)
        self.current_idx = -1

    def _run(self):
        for _ in range(len(self.DATA)):
            self._init_rand()
            while self.current_idx != len(self.DATA)-1:
                self._logic_handler()
            self._verify_trend()

    """
    Handles main logic of DBSCAN class, as described in documentation
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

    """
    Find all neighbours within epsilon of given data point
    """
    def _init_neighbours(self, data_point):
        neighbours = []

        for datum in self.DATA:
            if datum.val != data_point.val:
                if self._euclidean_dist(datum, data_point) <= self.epsilon:
                    neighbours.append(datum)
                    datum.add_neighbour(data_point)
                    data_point.add_neighbour(datum)

        return neighbours

    def _require_replacement(self, i, j):
        #  If point is outlier, and has neighbours => 50-50 split or worse
        #  Assumes the point itself is not considered its own neighbour
        return (self.DATA[i+j].type == 1) and (len(self.DATA[i+j].neighbours) > 0)

    def _best_approximate(self, r, i, j, data):
        prediction = r * self.DATA[i + j].xval + self.DATA[i].val
        #  Find value closest to predicted point
        min_diff = 10
        min_diff_datum = None
        for k in data:
            diff = abs(k.val - prediction)
            if diff < min_diff:
                min_diff = diff
                min_diff_datum = k

        return min_diff_datum

    def _adjust_classifier(self, data, closest_datum):
        for adjacent in data:
            if adjacent in closest_datum.neighbours:
                adjacent.type = 3
                self.CORE.add(adjacent)
                if adjacent in self.NOISE:
                    self.NOISE.remove(adjacent)
            else:
                adjacent.type = 1
                self.NOISE.add(adjacent)
        closest_datum.type = 3
        self.CORE.add(closest_datum)
        self.NOISE.remove(closest_datum)

    """
    Final method to be called after all data is clustered.
    Verifies the outliers are correctly chosen for each time interval, and acts accordingly.
    This is discussed in more detail in the paper, under 'Implementation Complications and Design Choices'
    """
    def _verify_trend(self):
        r_data = ([k.xval for k in self.DATA if k.type != 1], [k.val for k in self.DATA if k.type != 1])
        # outliers = [k for k in self.DATA if k.type == 1]
        #  Calculate correlation coefficient of non-outliers
        r = self._correlation_coefficient(r_data[0], r_data[1])

        #  Iterate through all time intervals.
        for i in range(0, len(self.DATA), self.num_imus):
            data = [self.DATA[k] for k in range(i, i + self.num_imus)]
            #  Check if there is a 50-50 split on outlier decision
            for j in range(self.num_imus):
                if self._require_replacement(i, j):
                    #  Then, find point closest (and neighbours) to predicted point (using correlation coeff)
                    closest_datum = self._best_approximate(r, i, j, data)
                    # If best approximate is classed as noise, swap noise and core types within interval
                    if closest_datum.type == 1:
                        self._adjust_classifier(data, closest_datum)

    @staticmethod
    def _correlation_coefficient(x_vals, y_vals):
        x_mean = sum(x_vals) / len(x_vals)
        y_mean = sum(y_vals) / len(y_vals)

        covar_vector = []
        for i in range(len(x_vals)):
            covar_vector.append((x_vals[i] - x_mean) * (y_vals[i] - y_mean))
        covar = sum(covar_vector)

        x_var_vector = []
        for i in range(len(x_vals)):
            x_var_vector.append((x_vals[i] - x_mean)**2)
        x_var = math.sqrt(sum(x_var_vector))

        y_var_vector = []
        for i in range(len(y_vals)):
            y_var_vector.append((y_vals[i] - y_mean) ** 2)
        y_var = math.sqrt(sum(y_var_vector))

        return covar / (x_var * y_var)

    @staticmethod
    def _euclidean_dist(p, q):
        return math.sqrt(
            sum((
                (p.val - q.val)**2,
                (p.xval - q.xval)**2,
            ))
        )

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
        self.neighbours = set()
        self.type = 0

    def add_neighbour(self, neighbour):
        self.neighbours.add(neighbour)


def data_reader(epsilon, restrict_size=20):
    with open("scattered_data.txt") as file:
        raw_data_arr = [line.split(' ') for line in file.readlines()][:restrict_size]
        data_arr = []

        for arr in raw_data_arr:
            prcsd_data = []
            for i in range(1, 5):
                data = arr[i]
                data.strip('\n')
                prcsd_data.append(float(data)) #+ float(data) * random.uniform(-0.02, 0.02))
            data_arr.append(prcsd_data)

    data_arr_x = [[i * epsilon*0.9] * 4 for i in range(len(data_arr))]

    return data_arr[:restrict_size], data_arr_x[:restrict_size]


def run():
    data_size = 30
    epsilon = 0.15/2  # Half of the average variance?
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


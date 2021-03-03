"""

Cyclic linked-list implementation for real-time moving data outlier detection calculations.
Concept:
    - New datum replaces oldest datum
    - Data struct can inform whether struct is populated

Implementation use-case example given in the main() method below.

"""


class LinkedList:
    def __init__(self, size):
        self.curr_node = None
        self.size = size
        self.tail_node = None
        # Data structure should only be used when fill == size (should be at least 'size' amount of data arrays)
        self.fill = 0

        self._setup()

    """
    Initialises the cyclic linked list with 0 values.
    IMPORTANT: Make sure self.curr_node points to the tail after execution.
    """
    def _setup(self):
        self.tail_node = Node(0)
        self.curr_node = self.tail_node
        for _ in range(self.size - 1):
            self._add_node(0)
        # Point back to tail, make it a cycle
        self.curr_node.next = self.tail_node

    """
    Private method for adding a new node to the linked list struct.
    """
    def _add_node(self, val):
        new_node = Node(val)
        self.curr_node.next = new_node
        self.curr_node = new_node

    """
    Public method emulating private 'add' method.
    This method actually overrides values in the pre-exiting nodes.
    """
    def add(self, val):
        self.curr_node.next.val = val
        self.curr_node = self.curr_node.next
        self.fill += 1

    """
    Retrieve the data in terms of array of arrays, standard format for outlier detection algorithms
    """
    def get_data(self):
        if not self.is_valid():
            print("Not enough data provided!")
            return
        arr = []
        temp_node = self.curr_node.next
        for _ in range(self.size):
            arr.append(temp_node.val)
            temp_node = temp_node.next

        return arr

    def is_valid(self):
        return self.fill >= self.size

    def __str__(self):
        temp_node = self.curr_node.next
        str_state = ""
        for _ in range(self.size):
            str_state += (" {} -> ".format(temp_node.val))
            temp_node = temp_node.next
        str_state += "\nStructure is valid: {}".format(self.is_valid())
        return str_state


class Node:
    def __init__(self, val):
        self.val = val
        self.next = None


def main():
    # Buffer of previous 4 data arrays
    buffer = LinkedList(4)
    #print(buffer)

    new_data_arr = [12, 14, 11, 13]
    buffer.add(new_data_arr)

    new_data_arr = [15, 17, 11, 14]
    buffer.add(new_data_arr)

    new_data_arr = [16, 18, 13, 15]
    buffer.add(new_data_arr)

    new_data_arr = [17, 19, 15, 16]
    buffer.add(new_data_arr)

    new_data_arr = [18, 20, 15, 17]
    buffer.add(new_data_arr)

    print(buffer)
    print(buffer.get_data())


main()

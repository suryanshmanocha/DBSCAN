#ifndef DBSCAN_HISTORIC_DATA_H
#define DBSCAN_HISTORIC_DATA_H

using namespace std;

namespace hd {
    typedef vector<float> data_arr;

    class Node {
    public:
        explicit Node(data_arr val);

        data_arr val = {0};
        Node *next = nullptr;
    };


    class HistoricData {
    public:
        explicit HistoricData(int size);

        void add(data_arr val);
        vector<data_arr> get_data();
        bool is_valid() const;
        void print();
    private:
        int size;
        int fill;
        Node* curr_node = nullptr;
        Node* tail_node = nullptr;

        void setup();
        void add_node();
    };
}

#endif //DBSCAN_HISTORIC_DATA_H
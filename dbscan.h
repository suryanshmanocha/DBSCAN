#ifndef DBSCAN_DBSCAN_H
#define DBSCAN_DBSCAN_H
#include <vector>

using namespace std;

namespace db {
    typedef vector<float> data_arr;

    class DataPoint {
    public:
        float val;
        float xval;
    };

    class DBSCAN {
    private:
        std::string DATA_TYPES[4] = {"Unknown", "Noise", "Border", "Core"};
    };

    class Node {
    public:
        Node(data_arr val);

        data_arr val = {0};
        Node *next = NULL;
    };

    class HistoricData {
    public:
        HistoricData(int size);

        void add(data_arr val);
        vector <data_arr> get_data();
        bool is_valid() const;
        void print();
    private:
        int size;
        int fill;
        Node* curr_node = NULL;
        Node* tail_node = NULL;

        void setup();
        void add_node();
    };
}

#endif //DBSCAN_DBSCAN_H
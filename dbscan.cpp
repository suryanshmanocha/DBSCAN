#include <iostream>
#include <string>
#include <vector>
#include "dbscan.h"

using namespace db;

Node::Node(data_arr val) {
    this->val = val;
}

HistoricData::HistoricData(int size) {
    this->size = size;
    fill = 0;
    setup();
}

void HistoricData::setup() {
    Node* root_node = new Node({0});
    tail_node = root_node;
    curr_node = tail_node;
    for (int i=0; i < size-1; i++) {
        add_node();
    }
    // Point back to tail, make it a cycle
    curr_node->next = tail_node;
}

void HistoricData::add_node() {
    Node* new_node = new Node({0});
    curr_node->next = new_node;
    curr_node = new_node;
}

bool HistoricData::is_valid() const {
    return fill >= size;
}

void HistoricData::add(data_arr val) {
    curr_node->next->val = val;
    curr_node = curr_node->next;
    fill++;
}

vector <data_arr> HistoricData::get_data() {
    if (!is_valid()){
        cout << "WARNING: " << "Not enough data provided! Only" << fill;
    }
    vector <data_arr> arr;
    struct Node *temp_node = curr_node->next;
    for (int i=0; i < size; i++){
        arr.push_back(temp_node->val);
        temp_node = temp_node->next;
    }
    return arr;
}

void HistoricData::print(){
    Node* temp_node = curr_node->next;
    string str_state;
    for (int i=0; i < size; i++){
        cout << "[";
        for (int i=0; i<temp_node->val.size();i++){
            cout << temp_node->val[i] << ",";
        }
        cout << "]";
        cout << " -> ";
        temp_node = temp_node->next;
    }
    printf("\nStructure is valid: %d", is_valid());
}


int main() {
    HistoricData buffer(4);
    buffer.add(vector <float> {3, 6, 8, 1});
    buffer.add(vector <float> {1, 2, 2, 3});
    buffer.add(vector <float> {2, 1, 1, 1});
    buffer.add(vector <float> {12, 11, 11, 11});
    buffer.add(vector <float> {22, 22, 21, 21});
    buffer.add(vector <float> {33, 33, 31, 31});
    buffer.print();

    return 0;
}
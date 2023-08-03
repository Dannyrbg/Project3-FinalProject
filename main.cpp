#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "JeopardyObj.h"
using namespace std;
using namespace chrono;

vector<questionData> readCSV(string filename) {
    vector<questionData> data;

    ifstream file(filename);

    string line;
    // skips first row
    if(getline(file, line)){
        while(getline(file, line)){
            istringstream iss(line);
            string temp;

            questionData row;

            getline(iss, row.category, '\t');

            getline(iss, temp, '\t');
            // initially all vals like 1,000 and 3,000 were 1 and 3
            temp.erase(remove(temp.begin(), temp.end(), ','), temp.end());
            row.value = stoi(temp);

            getline(iss, row.question, '\t');
            getline(iss, row.answer, '\t');

            data.push_back(row);
        }
    }

    file.close();
    return data;
}

void quickSort(vector<questionData>& data, int low, int high){
    // base case
    if(low < high){
        int pivotpos = low + (high - low) / 2;
        int pivotval = data[pivotpos].value;

        int i = low;
        int j = high;
        while(i <= j){
            while (data[i].value < pivotval) i++;
            while (data[j].value > pivotval) j--;
            if(i <= j){
                swap(data[i], data[j]);
                i++;
                j--;
            }
        }

        quickSort(data, low, j);
        quickSort(data, i, high);
    }
}

void merge(vector<questionData>& data, int low, int mid, int high){
    int leftSize = mid - low + 1;
    int rightSize = high - mid;

    vector<questionData> left(leftSize);
    vector<questionData> right(rightSize);

    for(int i = 0; i < leftSize; i++){
        left[i] = data[low + i];
    }

    for(int j = 0; j < rightSize; j++){
        right[j] = data[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = low;

    while(i < leftSize && j < rightSize){
        if(left[i].value <= right[j].value){
            data[k] = left[i];
            i++;
        }
        else{
            data[k] = right[j];
            j++;
        }
        k++;
    }

    while(i < leftSize){
        data[k] = left[i];
        i++;
        k++;
    }

    while(j < rightSize){
        data[k] = right[j];
        j++;
        k++;
    }
}

void mergeSort(vector<questionData>& data, int low, int high) {
    // base case
    if(low < high){
        int mid = low + (high - low) / 2;
        mergeSort(data, low, mid);
        mergeSort(data, mid + 1, high);
        merge(data, low, mid, high);
    }
}

int main() {
    string filename = "data.csv";
    vector<questionData> gameData = readCSV(filename);

    unordered_map<string, vector<questionData>> categoryMap;
    for(auto temp : gameData){
        categoryMap[temp.category].push_back(temp);
    }

    string category = "BEFORE & AFTER";
    if(categoryMap.find(category) != categoryMap.end()){
        vector<questionData> categoryData = categoryMap[category];

        /*auto mergeSortStart = high_resolution_clock::now();
        mergeSort(categoryData, 0, categoryData.size() - 1);
        auto mergeSortEnd = high_resolution_clock::now();
        duration<double> mergeSortDuration = mergeSortEnd - mergeSortStart;
        cout << "Merge Sort Time: " << mergeSortDuration.count() << " seconds" << endl;*/

        auto quickSortStart = high_resolution_clock::now();
        quickSort(categoryData, 0, categoryData.size() - 1);
        auto quickSortEnd = high_resolution_clock::now();
        duration<double> quickSortDuration = quickSortEnd - quickSortStart;
        cout << "Quick Sort Time: " << quickSortDuration.count() << " seconds" << endl;

        for (auto row : categoryData) {
            cout << "Value: " << row.value << endl;
            cout << "------------------------" << endl;
        }
    }
    else{
        cout << "Category not found." << endl;
    }
    return 0;
}

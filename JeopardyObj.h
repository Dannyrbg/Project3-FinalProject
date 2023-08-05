#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
using namespace std;

//Struct to hold each question and its inherent data
struct questionData {
    string category;
    int value;
    string question;
    string answer;
    bool shown = false;
};

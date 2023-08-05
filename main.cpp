#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "JeopardyObj.h"
#include <ctime>
#include <cstdlib>
#include <string>
using namespace std;
using namespace chrono;

vector<questionData> readCSV(string filename) {
    vector<questionData> data;

    // open file
    ifstream file(filename);

    string line;
    // skips first row
    if(getline(file, line)){
        while(getline(file, line)){
            istringstream iss(line);
            string temp;

            questionData row;
            // changed to to a tab separated file instead of comma since commas are in the questions
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
        // pivot middle
        int pivotpos = low + (high - low) / 2;
        int pivotval = data[pivotpos].value;

        int i = low;
        int j = high;
        // partition array
        while(i <= j){
            while(data[i].value < pivotval){i++;}
            while(data[j].value > pivotval){j--;}
            // swapping!
            if(i <= j){
                swap(data[i], data[j]);
                i++;
                j--;
            }
        }
        // recursively sort each parition
        quickSort(data, low, j);
        quickSort(data, i, high);
    }
}

void merge(vector<questionData>& data, int low, int mid, int high){
    // sizing
    int leftSize = mid - low + 1;
    int rightSize = high - mid;

    vector<questionData> left(leftSize);
    vector<questionData> right(rightSize);

    // copy data to appropriate subarrays
    for(int i = 0; i < leftSize; i++){
        left[i] = data[low + i];
    }

    for(int j = 0; j < rightSize; j++){
        right[j] = data[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = low;
    // merging arryas while comparing values in the arrays
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
    // remaining elements in arrays are copied into final array
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
        // middle!
        int mid = low + (high - low) / 2;
        // recursive calls
        mergeSort(data, low, mid);
        mergeSort(data, mid + 1, high);
        // put back together
        merge(data, low, mid, high);
    }
}

vector<questionData> ranSelector(vector<questionData> categoryData){
    vector<questionData> selectedQuestions;
    vector<int> selectedValues;

    srand(time(0));

    for(int i = 1; i < 6; i++){
        // increment values by 200 to match jeopardy board
        int value = i * 200;
        vector<questionData> possibleQuestions;
        for(questionData question : categoryData){
            if (question.value == value) {
                possibleQuestions.push_back(question);
            }
        }
        // sometimes it was empty
        if (!possibleQuestions.empty()) {
            // make sure it fits
            int randomIndex = rand() % possibleQuestions.size();
            selectedQuestions.push_back(possibleQuestions[randomIndex]);
        }
    }

    return selectedQuestions;
}

vector<vector<questionData>> selectBoard(unordered_map<string, vector<questionData>> categoryMap, bool sort, duration<double>& quickSortDuration, duration<double>& mergeSortDuration) {
    vector<vector<questionData>> boardQuestions;
    // this is where the categories are selected
    for(int j = 0; j < 6; j++){
        bool minimum = true;
        // make a copy so that we can run both sorts to compare times
        vector<questionData> categoryData;
        vector<questionData> categoryDataCopy;

        while (minimum) {
            // ran seed includes j to ensure sufficient difference in seeds for category selection
            srand(time(0) + j);
            int randomIndex = rand() % categoryMap.size();
            // checks if all of the values are present in a category (some small ones aren't)
            auto it = next(categoryMap.begin(), randomIndex);
            bool val200 = false;
            bool val400 = false;
            bool val600 = false;
            bool val800 = false;
            bool val1000 = false;
            for (int i = 0; i < it -> second.size(); ++i) {
                if (it->second[i].value == 200) {
                    val200 = true;
                }
                if (it->second[i].value == 400) {
                    val400 = true;
                }
                if (it->second[i].value == 600) {
                    val600 = true;
                }
                if (it->second[i].value == 800) {
                    val800 = true;
                }
                if (it->second[i].value == 1000) {
                    val1000 = true;
                }
            }
            if (val200 && val400 && val600 && val800 && val1000) {
                minimum = false;
                categoryData = it -> second;
            }
        }
        // handles passing through sorting, first sort is actually used for questions, second is just for comparison
        // also times the sorting with chrono::
        if(sort) {
            categoryDataCopy = categoryData;
            auto quickSortStart = high_resolution_clock::now();
            quickSort(categoryData, 0, categoryData.size() - 1);
            auto quickSortEnd = high_resolution_clock::now();
            quickSortDuration = quickSortEnd - quickSortStart;

            auto mergeSortStart = high_resolution_clock::now();
            mergeSort(categoryDataCopy, 0, categoryDataCopy.size() - 1);
            auto mergeSortEnd = high_resolution_clock::now();
            mergeSortDuration = mergeSortEnd - mergeSortStart;
        }
        else{
            categoryDataCopy = categoryData;

            auto mergeSortStart = high_resolution_clock::now();
            mergeSort(categoryData, 0, categoryData.size() - 1);
            auto mergeSortEnd = high_resolution_clock::now();
            mergeSortDuration = mergeSortEnd - mergeSortStart;

            auto quickSortStart = high_resolution_clock::now();
            quickSort(categoryDataCopy, 0, categoryDataCopy.size() - 1);
            auto quickSortEnd = high_resolution_clock::now();
            quickSortDuration = quickSortEnd - quickSortStart;
        }
        // categoryData are passed into ranSelector to select questions from sorted categories
        vector<questionData> selectedQuestions = ranSelector(categoryData);
        boardQuestions.push_back(selectedQuestions);
    }
    return boardQuestions;
}

int main() {
    string filename = "data.csv";
    vector<questionData> gameData = readCSV(filename);
    // stores all the items in the gameData by category
    unordered_map<string, vector<questionData>> categoryMap;
    for(auto temp : gameData){
        categoryMap[temp.category].push_back(temp);
    }

    //BOARD STUFF
    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::RenderWindow window(sf::VideoMode(1000, 800), "Jeopardy");
    // all of this until the next comment is setting up welcome window text and recs
    sf::Text titleWelcome;
    titleWelcome.setString("WELCOME TO JEOPARDY!");
    titleWelcome.setFont(font);
    titleWelcome.setStyle(sf::Text::Bold | sf::Text::Underlined);
    titleWelcome.setFillColor(sf::Color::White);
    titleWelcome.setCharacterSize(36);

    float cenX = window.getSize().x / 2.0f;
    float cenY = window.getSize().y / 2.0f;

    sf::FloatRect bounds = titleWelcome.getGlobalBounds();
    titleWelcome.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    titleWelcome.setPosition(cenX, cenY - 150);

    sf::Text selectAlg;
    selectAlg.setString("Select Sorting Algorithm:");
    selectAlg.setFont(font);
    selectAlg.setStyle(sf::Text::Bold);
    selectAlg.setFillColor(sf::Color::White);
    selectAlg.setCharacterSize(26);

    sf::FloatRect boundsAlg = selectAlg.getGlobalBounds();
    selectAlg.setOrigin(boundsAlg.left + boundsAlg.width / 2.0f, boundsAlg.top + boundsAlg.height / 2.0f);
    selectAlg.setPosition(cenX, cenY - 75);

    sf::Text QuickSortBox;
    QuickSortBox.setString("Quick Sort");
    QuickSortBox.setFont(font);
    QuickSortBox.setFillColor(sf::Color::White);
    QuickSortBox.setCharacterSize(24);

    sf::FloatRect boundsQuick = QuickSortBox.getGlobalBounds();
    QuickSortBox.setOrigin(boundsQuick.left + boundsQuick.width / 2.0f, boundsQuick.top + boundsQuick.height / 2.0f);
    QuickSortBox.setPosition(cenX + 150, cenY + 60);

    sf::Vector2f quickRec(boundsQuick.width + 30, boundsQuick.height + 30);
    sf::RectangleShape quickBox(quickRec);
    quickBox.setOutlineColor(sf::Color::White);
    quickBox.setFillColor(sf::Color::Transparent);
    quickBox.setOutlineThickness(2);
    quickBox.setPosition(cenX - quickRec.x / 2.0f + 150, cenY - quickRec.y / 2.0f + 60);

    sf::Text MergeSortBox;
    MergeSortBox.setString("Merge Sort");
    MergeSortBox.setFont(font);
    MergeSortBox.setFillColor(sf::Color::White);
    MergeSortBox.setCharacterSize(24);

    sf::FloatRect boundsMerge = MergeSortBox.getGlobalBounds();
    MergeSortBox.setOrigin(boundsMerge.left + boundsMerge.width / 2.0f, boundsMerge.top + boundsMerge.height / 2.0f);
    MergeSortBox.setPosition(cenX - 150, cenY + 60);

    sf::Vector2f mergeRec(boundsMerge.width + 30, boundsMerge.height + 30);
    sf::RectangleShape mergeBox(mergeRec);
    mergeBox.setOutlineColor(sf::Color::White);
    mergeBox.setFillColor(sf::Color::Transparent);
    mergeBox.setOutlineThickness(2);
    mergeBox.setPosition(cenX - mergeRec.x / 2.0f - 150, cenY - mergeRec.y / 2.0f + 60);

    // init variables we need to use within the game handling loops
    bool sort = false;
    bool switchToBoard = false;
    bool isOpen = true;
    bool alreadySorted = false;
    vector<vector<questionData>> boardQuestions;
    duration<double> quickSortDuration;
    duration<double> mergeSortDuration;

    while (isOpen) {
        sf::Event event;
        // welcome window event handling
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                isOpen = false;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                int xCor = event.mouseButton.x;
                int yCor = event.mouseButton.y;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::FloatRect quickSortButton = quickBox.getGlobalBounds();
                    if (quickSortButton.contains((float)xCor, (float)yCor)) {
                        switchToBoard = true;
                        // used to tell which sort is dictating the actual questions
                        sort = true;
                    }
                    sf::FloatRect mergeSortButton = mergeBox.getGlobalBounds();
                    if (mergeSortButton.contains((float)xCor, (float)yCor)) {
                        switchToBoard = true;
                        // used to tell which sort is dictating the actual questions
                        sort = false;
                    }
                }
            }
        }
        window.clear(sf::Color::Blue);
        window.draw(titleWelcome);
        window.draw(selectAlg);
        window.draw(QuickSortBox);
        window.draw(quickBox);
        window.draw(MergeSortBox);
        window.draw(mergeBox);
        window.display();
        // welcome window has been left, board opens now
        if (switchToBoard) {
            window.close();
            // this is where board questions are selected and sorted
            // without this if statement, it evaluates it every time the while loop re runs, destroys performance
            if(!alreadySorted) {
                boardQuestions = selectBoard(categoryMap, sort, quickSortDuration, mergeSortDuration);
                alreadySorted = true;
            }

            // rendering board
            int rows = 6;
            int cols = 6;
            int boxWidth = 200;
            int boxHeight= 150;
            sf::RenderWindow boardWindow(sf::VideoMode(cols * boxWidth, rows * boxHeight + 100), "Jeopardy");

            //Text sprite representing the matrix of the board
            sf::Text grid[6][6];

            // render categories in row 1
            for(int j = 0; j < 6; j++){
                grid[0][j].setFont(font);
                grid[0][j].setCharacterSize(10);
                grid[0][j].setStyle(sf::Text::Bold);
                grid[0][j].setFillColor(sf::Color::Yellow);

                grid[0][j].setString(boardQuestions[j][0].category);
            }
            // render value displays for board
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    //For every value outside of the category row itself
                    if (i > 0) {
                        grid[i][j].setFont(font);
                        grid[i][j].setCharacterSize(22);
                        grid[i][j].setStyle(sf::Text::Bold);
                        grid[i][j].setFillColor(sf::Color::Yellow);
                    }

                    if (i == 1) {
                        grid[i][j].setString("$200");
                    }
                    if (i == 2) {
                        grid[i][j].setString("$400");
                    }
                    if (i == 3) {
                        grid[i][j].setString("$600");
                    }
                    if (i == 4) {
                        grid[i][j].setString("$800");
                    }
                    if (i == 5) {
                        grid[i][j].setString("$1000");
                    }

                    sf::FloatRect textBounds = grid[i][j].getLocalBounds();
                    float cenX = j * boxWidth + (boxWidth - textBounds.width) / 2.0f;
                    float cenY = i * boxHeight + (boxHeight - textBounds.height) / 2.0f;
                    grid[i][j].setPosition(cenX, cenY);
                }
            }

            //Create question boolean to know when to open question window and coordinate indices
            bool DisplayQuestion = false;
            int indX = 0;
            int indY = 0;

            // board event handling
            while (boardWindow.isOpen()) {
                sf::Event event;
                while (boardWindow.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        isOpen = false;
                        boardWindow.close();
                    }
                    // handles clicking on questions
                    else if (event.type == sf::Event::MouseButtonPressed) {
                        int xCor = event.mouseButton.x;
                        int yCor = event.mouseButton.y;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (yCor > 150 && yCor < 900) {
                                DisplayQuestion = true;
                                //Find x-coordinate
                                if (xCor < 200) {
                                    indX = 0;
                                }
                                else if (200 < xCor && xCor < 400) {
                                    indX = 1;
                                }
                                else if (400 < xCor && xCor < 600) {
                                    indX = 2;
                                }
                                else if (600 < xCor && xCor < 800) {
                                    indX = 3;
                                }
                                else if (800 < xCor && xCor < 1000) {
                                    indX = 4;
                                }
                                else if (1000 < xCor && xCor < 1200) {
                                    indX = 5;
                                }
                                //Find y-coordinate
                                if (yCor < 300) {
                                    indY = 0;
                                }
                                if (yCor > 300 && yCor < 450) {
                                    indY = 1;
                                }
                                if (yCor > 450 && yCor < 600) {
                                    indY = 2;
                                }
                                if (yCor > 600 && yCor < 750) {
                                    indY = 3;
                                }
                                if (yCor > 750 && yCor < 900) {
                                    indY = 4;
                                }
                            }
                        }
                    }
                    boardWindow.clear(sf::Color::Blue);
                    // sets up and draws the grid for the board
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j) {
                            sf::RectangleShape gridBox(sf::Vector2f(boxWidth, boxHeight));
                            gridBox.setPosition(j * boxWidth, i * boxHeight);
                            gridBox.setOutlineThickness(2);
                            gridBox.setOutlineColor(sf::Color::Black);
                            gridBox.setFillColor(sf::Color::Transparent);
                            boardWindow.draw(gridBox);
                        }
                    }
                    //Draw the question values
                    for (int i = 0; i < rows; ++i) {
                        for (int j = 0; j < cols; ++j) {
                            boardWindow.draw(grid[i][j]);
                        }
                    }

                    // these times are calculated in selectBoard function, here we modify values for display
                    sf::Text mergeDuration;
                    float mergeSortTimeFinal = static_cast<float>(mergeSortDuration.count()) * 100000;
                    mergeDuration.setString("Merge Sort Duration: " + to_string(mergeSortTimeFinal) + " ms");
                    mergeDuration.setFont(font);
                    mergeDuration.setFillColor(sf::Color::White);
                    mergeDuration.setCharacterSize(20);

                    sf::FloatRect boundsMergeDur = mergeDuration.getGlobalBounds();
                    mergeDuration.setOrigin(boundsMergeDur.left + boundsMergeDur.width / 2.0f, boundsMergeDur.top + boundsMergeDur.height / 2.0f);
                    mergeDuration.setPosition(cenX - 250, rows * boxHeight + 50);

                    boardWindow.draw(mergeDuration);

                    sf::Text quickDuration;
                    float quickSortTimeFinal = static_cast<float>(quickSortDuration.count()) * 1000000;
                    quickDuration.setString("Quick Sort Duration: " + to_string(quickSortTimeFinal) + " ms");
                    quickDuration.setFont(font);
                    quickDuration.setFillColor(sf::Color::White);
                    quickDuration.setCharacterSize(20);

                    sf::FloatRect boundsQuickDur = quickDuration.getGlobalBounds();
                    quickDuration.setOrigin(boundsQuickDur.left + boundsQuickDur.width / 2.0f, boundsQuickDur.top + boundsQuickDur.height / 2.0f);
                    quickDuration.setPosition(cenX + 450, rows * boxHeight + 50);

                    boardWindow.draw(quickDuration);

                    boardWindow.display();

                    //Display Question Window
                    if (DisplayQuestion && !boardQuestions[indX][indY].shown) {
                        sf::RenderWindow questionWindow(sf::VideoMode(cols * boxWidth - 400, rows * boxHeight - 300), "Jeopardy Question");
                        //Have that index in the matrix have its shown value set true to avoid being able to select the
                        //question again
                        boardQuestions[indX][indY].shown = true;

                        //Since quesiton will have been displayed, clear the value display for that question
                        grid[indY + 1][indX].setString("");

                        string questionString = boardQuestions[indX][indY].question;
                        bool needExtraString = false;
                        bool displayAns = false;
                        string extraString = "";
                        //Modify extra string if initial question is too long to fit
                        if (questionString.length() > 70) {
                            extraString = questionString.substr(69);
                            if (extraString[0] != ' ') {
                                extraString = "-" + extraString;
                            }
                            needExtraString = true;
                        }

                        //Create question text sprite
                        sf::Text questionText;
                        if (needExtraString) {
                            questionText.setString(questionString.substr(0, 69));
                        }
                        else {
                            questionText.setString(questionString);
                        }
                        questionText.setFont(font);
                        questionText.setFillColor(sf::Color::White);
                        questionText.setCharacterSize(18);

                        float cenX = questionWindow.getSize().x / 2.0f;
                        float cenY = questionWindow.getSize().y / 2.0f;

                        //Position question on board
                        sf::FloatRect bounds = questionText.getGlobalBounds();
                        questionText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
                        questionText.setPosition(cenX, cenY - 25);

                        sf::Text extraText;
                        extraText.setString(extraString);
                        extraText.setFont(font);
                        extraText.setFillColor(sf::Color::White);
                        extraText.setCharacterSize(18);

                        sf::FloatRect boundsExtra = extraText.getGlobalBounds();
                        extraText.setOrigin(boundsExtra.left + boundsExtra.width / 2.0f, boundsExtra.top + boundsExtra.height / 2.0f);
                        extraText.setPosition(cenX, cenY + 25);

                        //Create an answer sprite and box
                        sf::Text AnswerBox;
                        AnswerBox.setString("ANSWER");
                        AnswerBox.setFont(font);
                        AnswerBox.setStyle(sf::Text::Bold);
                        AnswerBox.setFillColor(sf::Color::White);
                        AnswerBox.setCharacterSize(22);

                        sf::FloatRect boundsAns = AnswerBox.getGlobalBounds();
                        AnswerBox.setOrigin(boundsAns.left + boundsAns.width / 2.0f, boundsAns.top + boundsAns.height / 2.0f);
                        AnswerBox.setPosition(cenX, cenY + 90);

                        sf::Vector2f AnsRec(boundsAns.width + 30, boundsAns.height + 30);
                        sf::RectangleShape AnsBox(AnsRec);
                        AnsBox.setOutlineColor(sf::Color::White);
                        AnsBox.setFillColor(sf::Color::Transparent);
                        AnsBox.setOutlineThickness(2);
                        AnsBox.setPosition(cenX - AnsRec.x / 2.0f, cenY - AnsRec.y / 2.0f + 90);

                        //Run events in the question window
                        while (questionWindow.isOpen()) {
                            sf::Event event;
                            while (questionWindow.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    //Set display question boolean to false to exit question window and return
                                    //to the board
                                    DisplayQuestion = false;
                                    questionWindow.close();
                                }
                                //The case where the answer button is clicked
                                if (event.type == sf::Event::MouseButtonPressed) {
                                    int xCor = event.mouseButton.x;
                                    int yCor = event.mouseButton.y;
                                    if (event.mouseButton.button == sf::Mouse::Left) {
                                        sf::FloatRect ansButton = AnsBox.getGlobalBounds();
                                        //Set the display answer boolean as true to display answer window
                                        if (ansButton.contains((float)xCor, (float)yCor)) {
                                            displayAns = true;
                                        }
                                    }
                                }
                            }
                            //Draw all question window sprites
                            questionWindow.clear(sf::Color::Blue);
                            questionWindow.draw(questionText);
                            questionWindow.draw(extraText);
                            questionWindow.draw(AnswerBox);
                            questionWindow.draw(AnsBox);
                            questionWindow.display();

                            //Display Answer Window
                            if (displayAns) {
                                sf::RenderWindow answerWindow(sf::VideoMode(cols * boxWidth - 500, rows * boxHeight - 400), "Jeopardy Answer");
                                //Same text operations as question except with answer value
                                string answerString = boardQuestions[indX][indY].answer;
                                bool needExtraString = false;
                                string extraString = "";
                                if (answerString.length() > 70) {
                                    extraString = answerString.substr(69);
                                    if (extraString[0] != ' ') {
                                        extraString = "-" + extraString;
                                    }
                                    needExtraString = true;
                                }

                                sf::Text answerText;
                                if (needExtraString) {
                                    answerText.setString(answerString.substr(0, 69));
                                }
                                else {
                                    answerText.setString(answerString);
                                }
                                answerText.setFont(font);
                                answerText.setStyle(sf::Text::Bold);
                                answerText.setFillColor(sf::Color::White);
                                answerText.setCharacterSize(26);

                                //Center the answer in the middle of the board
                                float cenX = answerWindow.getSize().x / 2.0f;
                                float cenY = answerWindow.getSize().y / 2.0f;

                                sf::FloatRect bounds = answerText.getGlobalBounds();
                                answerText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
                                answerText.setPosition(cenX, cenY);

                                sf::Text extraText;
                                extraText.setString(extraString);
                                extraText.setFont(font);
                                extraText.setFillColor(sf::Color::White);
                                extraText.setCharacterSize(18);

                                sf::FloatRect boundsExtra = extraText.getGlobalBounds();
                                extraText.setOrigin(boundsExtra.left + boundsExtra.width / 2.0f, boundsExtra.top + boundsExtra.height / 2.0f);
                                extraText.setPosition(cenX, cenY + 25);

                                //Run events of answer window
                                while (answerWindow.isOpen()) {
                                    sf::Event event;
                                    //Only operation is closing the window after seeing the answer
                                    while (answerWindow.pollEvent(event)) {
                                        if (event.type == sf::Event::Closed) {
                                            //Set display answer boolean as false to exit answer window
                                            displayAns = false;
                                            answerWindow.close();
                                        }
                                    }
                                    //Display all answer window sprites
                                    answerWindow.clear(sf::Color::Blue);
                                    answerWindow.draw(answerText);
                                    answerWindow.draw(extraText);
                                    answerWindow.display();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/*
 * Project Two - ABCU Advising Assistance Program
 * Author: Lufranc Kousse
 *
 * This program reads course information from a CSV file and stores the data in
 * a vector of Course objects. It allows an advisor to load course data, print
 * the full course list in alphanumeric order, and search for a specific course
 * to view its title and prerequisites.
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Course structure stores one course record from the input file.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Removes spaces from the beginning and end of a string.
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

// Converts a string to uppercase so course searches are consistent.
string toUpperCase(string text) {
    for (char& character : text) {
        character = static_cast<char>(toupper(character));
    }
    return text;
}

// Splits one CSV line into separate values.
vector<string> splitCSVLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream stream(line);

    while (getline(stream, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// Searches the vector for a course number and returns the index.
// Returns -1 if the course is not found.
int findCourseIndex(const vector<Course>& courses, const string& courseNumber) {
    string searchNumber = toUpperCase(courseNumber);

    for (int i = 0; i < static_cast<int>(courses.size()); ++i) {
        if (toUpperCase(courses.at(i).courseNumber) == searchNumber) {
            return i;
        }
    }

    return -1;
}

// Checks whether every prerequisite listed in the file also exists as a course.
bool validatePrerequisites(const vector<Course>& courses) {
    for (const Course& course : courses) {
        for (const string& prerequisite : course.prerequisites) {
            if (findCourseIndex(courses, prerequisite) == -1) {
                cout << "Error: Prerequisite " << prerequisite
                     << " listed for " << course.courseNumber
                     << " does not exist in the course file." << endl;
                return false;
            }
        }
    }

    return true;
}

// Loads course data from a CSV file into the vector.
bool loadCourses(const string& fileName, vector<Course>& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << fileName << endl;
        return false;
    }

    vector<Course> loadedCourses;
    string line;

    while (getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitCSVLine(line);

        // Each line must have at least a course number and title.
        if (tokens.size() < 2) {
            cout << "Error: Invalid file format. Each line must contain "
                 << "a course number and course title." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens.at(0));
        course.courseTitle = tokens.at(1);

        if (course.courseNumber.empty() || course.courseTitle.empty()) {
            cout << "Error: Course number and course title cannot be blank." << endl;
            return false;
        }

        // Any remaining values on the line are prerequisites.
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens.at(i).empty()) {
                course.prerequisites.push_back(toUpperCase(tokens.at(i)));
            }
        }

        loadedCourses.push_back(course);
    }

    file.close();

    if (loadedCourses.empty()) {
        cout << "Error: No course data was loaded from the file." << endl;
        return false;
    }

    if (!validatePrerequisites(loadedCourses)) {
        return false;
    }

    courses = loadedCourses;
    cout << "Courses loaded successfully." << endl;
    return true;
}

// Prints all courses in alphanumeric order by course number.
void printCourseList(vector<Course> courses) {
    if (courses.empty()) {
        cout << "No courses are loaded. Please load course data first." << endl;
        return;
    }

    sort(courses.begin(), courses.end(), [](const Course& left, const Course& right) {
        return left.courseNumber < right.courseNumber;
    });

    cout << endl;
    cout << "Here is a sample schedule:" << endl;
    cout << endl;

    for (const Course& course : courses) {
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }
}

// Prints one course and its prerequisites.
void printCourseInformation(const vector<Course>& courses, const string& courseNumber) {
    if (courses.empty()) {
        cout << "No courses are loaded. Please load course data first." << endl;
        return;
    }

    int index = findCourseIndex(courses, courseNumber);

    if (index == -1) {
        cout << "Course not found." << endl;
        return;
    }

    const Course& course = courses.at(index);

    cout << course.courseNumber << ", " << course.courseTitle << endl;
    cout << "Prerequisites: ";

    if (course.prerequisites.empty()) {
        cout << "None" << endl;
        return;
    }

    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        string prerequisiteNumber = course.prerequisites.at(i);
        int prerequisiteIndex = findCourseIndex(courses, prerequisiteNumber);

        if (prerequisiteIndex != -1) {
            cout << courses.at(prerequisiteIndex).courseNumber;
        }
        else {
            cout << prerequisiteNumber;
        }

        if (i < course.prerequisites.size() - 1) {
            cout << ", ";
        }
    }

    cout << endl;
}

// Displays the menu options for the advisor.
void displayMenu() {
    cout << endl;
    cout << "Menu:" << endl;
    cout << "  1. Load Data Structure" << endl;
    cout << "  2. Print Course List" << endl;
    cout << "  3. Print Course" << endl;
    cout << "  9. Exit" << endl;
    cout << endl;
    cout << "What would you like to do? ";
}

int main() {
    vector<Course> courses;
    string fileName;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        switch (choice) {
            case 1:
                cout << "Enter the course data file name: ";
                getline(cin, fileName);
                loadCourses(fileName, courses);
                break;

            case 2:
                printCourseList(courses);
                break;

            case 3: {
                string courseNumber;
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                printCourseInformation(courses, courseNumber);
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}

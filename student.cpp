/*
    ============================================================
     STUDENT MANAGEMENT SYSTEM
    ============================================================
    A console-based, menu-driven Student Management System
    written in C++ that supports:
        - Adding student records
        - Updating student records
        - Deleting student records
        - Displaying all records
        - Searching for a record
        - Persistent storage using file handling (text file)

    Data is stored in "students.txt" in the same directory as
    the compiled executable, using '|' as a field delimiter.
    ============================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const string DATA_FILE = "students.txt";

// ------------------------------------------------------------
// Student record structure
// ------------------------------------------------------------
struct Student {
    int id;
    string name;
    int age;
    string course;
    double marks;
};

// ------------------------------------------------------------
// Utility: clear bad input state from cin
// ------------------------------------------------------------
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------------------------------------------
// Utility: read an integer safely
// ------------------------------------------------------------
int readInt(const string &prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a whole number.\n";
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    }
}

// ------------------------------------------------------------
// Utility: read a double safely
// ------------------------------------------------------------
double readDouble(const string &prompt) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    }
}

// ------------------------------------------------------------
// Utility: read a full line (allows spaces) safely
// ------------------------------------------------------------
string readLine(const string &prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    while (value.empty()) {
        cout << "This field cannot be empty. " << prompt;
        getline(cin, value);
    }
    return value;
}

// ------------------------------------------------------------
// StudentManager: handles all operations + file persistence
// ------------------------------------------------------------
class StudentManager {
private:
    vector<Student> students;

    // Replace '|' in free text fields to keep file format safe
    static string sanitize(string s) {
        replace(s.begin(), s.end(), '|', '/');
        return s;
    }

public:
    StudentManager() {
        loadFromFile();
    }

    // ---------------- File Handling ----------------

    void loadFromFile() {
        students.clear();
        ifstream fin(DATA_FILE);
        if (!fin) return; // file may not exist yet on first run

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string token;
            Student s;

            getline(ss, token, '|'); s.id = stoi(token);
            getline(ss, token, '|'); s.name = token;
            getline(ss, token, '|'); s.age = stoi(token);
            getline(ss, token, '|'); s.course = token;
            getline(ss, token, '|'); s.marks = stod(token);

            students.push_back(s);
        }
        fin.close();
    }

    void saveToFile() const {
        ofstream fout(DATA_FILE, ios::trunc);
        for (const auto &s : students) {
            fout << s.id << '|' << s.name << '|' << s.age << '|'
                 << s.course << '|' << s.marks << '\n';
        }
        fout.close();
    }

    // ---------------- Core Operations ----------------

    bool idExists(int id) const {
        return any_of(students.begin(), students.end(),
                       [id](const Student &s) { return s.id == id; });
    }

    void addStudent() {
        Student s;
        s.id = readInt("Enter Student ID: ");
        if (idExists(s.id)) {
            cout << "A student with ID " << s.id << " already exists.\n";
            return;
        }
        s.name = sanitize(readLine("Enter Student Name: "));
        s.age = readInt("Enter Student Age: ");
        s.course = sanitize(readLine("Enter Course: "));
        s.marks = readDouble("Enter Marks (0-100): ");

        students.push_back(s);
        saveToFile();
        cout << "Student added successfully!\n";
    }

    void updateStudent() {
        int id = readInt("Enter Student ID to update: ");
        auto it = find_if(students.begin(), students.end(),
                           [id](const Student &s) { return s.id == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }

        cout << "Leave a field blank (press Enter) to keep its current value.\n";

        cout << "Current Name: " << it->name << "\nNew Name: ";
        string input;
        getline(cin, input);
        if (!input.empty()) it->name = sanitize(input);

        cout << "Current Age: " << it->age << "\nNew Age (or blank): ";
        getline(cin, input);
        if (!input.empty()) it->age = stoi(input);

        cout << "Current Course: " << it->course << "\nNew Course (or blank): ";
        getline(cin, input);
        if (!input.empty()) it->course = sanitize(input);

        cout << "Current Marks: " << it->marks << "\nNew Marks (or blank): ";
        getline(cin, input);
        if (!input.empty()) it->marks = stod(input);

        saveToFile();
        cout << "Student record updated successfully!\n";
    }

    void deleteStudent() {
        int id = readInt("Enter Student ID to delete: ");
        auto it = find_if(students.begin(), students.end(),
                           [id](const Student &s) { return s.id == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }

        cout << "Are you sure you want to delete " << it->name
             << " (ID: " << it->id << ")? (y/n): ";
        char confirm;
        cin >> confirm;
        clearInputBuffer();

        if (confirm == 'y' || confirm == 'Y') {
            students.erase(it);
            saveToFile();
            cout << "Student deleted successfully!\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    }

    void searchStudent() const {
        int id = readInt("Enter Student ID to search: ");
        auto it = find_if(students.begin(), students.end(),
                           [id](const Student &s) { return s.id == id; });

        if (it == students.end()) {
            cout << "No student found with ID " << id << ".\n";
            return;
        }
        printHeader();
        printRow(*it);
    }

    static void printHeader() {
        cout << left
             << setw(8)  << "ID"
             << setw(20) << "Name"
             << setw(6)  << "Age"
             << setw(20) << "Course"
             << setw(8)  << "Marks" << "\n";
        cout << string(62, '-') << "\n";
    }

    static void printRow(const Student &s) {
        cout << left
             << setw(8)  << s.id
             << setw(20) << s.name
             << setw(6)  << s.age
             << setw(20) << s.course
             << setw(8)  << fixed << setprecision(2) << s.marks << "\n";
    }

    void displayAll() const {
        if (students.empty()) {
            cout << "No student records found.\n";
            return;
        }
        printHeader();
        for (const auto &s : students) printRow(s);
        cout << "\nTotal Students: " << students.size() << "\n";
    }
};

// ------------------------------------------------------------
// Menu display
// ------------------------------------------------------------
void showMenu() {
    cout << "\n============================================\n";
    cout << "        STUDENT MANAGEMENT SYSTEM\n";
    cout << "============================================\n";
    cout << "1. Add Student\n";
    cout << "2. Update Student\n";
    cout << "3. Delete Student\n";
    cout << "4. Display All Students\n";
    cout << "5. Search Student by ID\n";
    cout << "6. Exit\n";
    cout << "============================================\n";
}

// ------------------------------------------------------------
// Main driver
// ------------------------------------------------------------
int main() {
    StudentManager manager;
    int choice;

    do {
        showMenu();
        choice = readInt("Enter your choice (1-6): ");

        switch (choice) {
            case 1: manager.addStudent();     break;
            case 2: manager.updateStudent();  break;
            case 3: manager.deleteStudent();  break;
            case 4: manager.displayAll();     break;
            case 5: manager.searchStudent();  break;
            case 6: cout << "Exiting... Data has been saved. Goodbye!\n"; break;
            default: cout << "Invalid choice. Please select 1-6.\n";
        }
    } while (choice != 6);

    return 0;
}
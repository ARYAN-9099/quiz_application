#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <windows.h>
using namespace std;

// Global variable to store quizzes
unordered_map<string, vector<pair<string, pair<vector<string>, int>>>>
    quizzes; // Moved outside of any class

// Function to display a quiz in a text file
void displayQuiz(const string &quizTitle, const string &filename) {
  if (quizzes.find(quizTitle) == quizzes.end()) {
    cout << "Quiz not found!\n";
    return;
  }

  ofstream outfile(filename);
  if (!outfile.is_open()) {
    cout << "Error opening file: " << filename << endl;
    return;
  }

  const auto &questions = quizzes[quizTitle];
  for (size_t i = 0; i < questions.size(); ++i) {
    outfile << "Q" << i + 1 << ": " << questions[i].first << endl;
    const auto &options = questions[i].second.first;
    for (size_t j = 0; j < options.size(); ++j) {
      outfile << j + 1 << ". " << options[j] << endl;
    }
    outfile << endl;
  }

  outfile.close();
  cout << "Quiz displayed in file: " << filename << endl;
}

// Function to save quizzes to a file
void save_quizzes() {
    ofstream quizFile("quizzes.txt");
    if (!quizFile.is_open()) {
        cout << "Error opening quizzes file for writing.\n";
        return;
    }

    for (const auto &quizPair : quizzes) {
        quizFile << quizPair.first << endl; // Quiz title
        quizFile << quizPair.second.size() << endl; // Number of questions
        for (const auto &questionPair : quizPair.second) {
            quizFile << questionPair.first << endl; // Question text
            const auto &options = questionPair.second.first;
            for (const auto &option : options) {
                quizFile << option << endl; // Each option
            }
            quizFile << questionPair.second.second << endl; // Correct option(s)
        }
    }
    quizFile.close();
}

// Function to load quizzes from a file
void load_quizzes() {
    ifstream quizFile("quizzes.txt");
    if (!quizFile.is_open()) {
        cout << "No quizzes found to load.\n";
        return;
    }

    string quizTitle;
    while (getline(quizFile, quizTitle)) {
        int numQuestions;
        quizFile >> numQuestions;
        quizFile.ignore(); // Ignore the newline after the number
        vector<pair<string, pair<vector<string>, int>>> questions;

        for (int i = 0; i < numQuestions; ++i) {
            string questionText;
            getline(quizFile, questionText);

            vector<string> options;
            for (int j = 0; j < 4; ++j) {
                string option;
                getline(quizFile, option);
                options.push_back(option);
            }

            int correctOption;
            quizFile >> correctOption;
            quizFile.ignore(); // Ignore the newline after the number

            questions.push_back({questionText, {options, correctOption}});
        }
        quizzes[quizTitle] = questions;
    }
    quizFile.close();
}

class User {
protected:
  string username;
  string password;

public:
  User(string u, string p) : username(u), password(p) {}

  virtual bool login(string u, string p) {
    return username == u && password == p;
  }

  virtual void menu() = 0; // Pure virtual function for role-specific menus

  string getPassword() const { return password; }
  virtual ~User() {} // Added function
  // Added function
};
class Student : public User {
private:
  unordered_set<string> enrolledCourses; // New member variable
  unordered_map<string, int> scores; //  To store scores for each quiz

public:
  Student(string u, string p) : User(u, p) {}

  void menu() override {
    cout << "Student Menu\n";
    cout << "1. Take Quiz\n";
    cout << "2. View Scores\n";
    cout << "3. View Enrolled Courses\n"; // New option
    cout << "4. Display Quiz\n";          // New option
    cout << "0. Logout\n";
  }

  void viewEnrolledCourses() {
    if (enrolledCourses.empty()) {
      cout << "You are not enrolled in any courses.\n";
    } else {
      cout << "Enrolled Courses:\n";
      for (const auto &course : enrolledCourses) {
        cout << "- " << course << "\n";
      }
    }
  }

  void addCourse(const string &course) { enrolledCourses.insert(course); }

  const unordered_set<string> &getEnrolledCourses() const {
    return enrolledCourses;
  }

  void displayQuiz(const string &quizTitle) {
    // Pass the 'quizzes' map from the QuizSystem
    ::displayQuiz(quizTitle, quizTitle + ".txt"); // Use the external function
  }

  // Function to take a quiz
void takeQuiz(const string &quizTitle, unordered_map<string, User *> &users) {
    if (quizzes.find(quizTitle) == quizzes.end()) {
        cout << "Quiz not found!\n";
        return;
    }

    const auto &questions = quizzes[quizTitle];
    vector<vector<int>> userAnswers; // Store user's multiple answers for each question
    int totalScore = 0;             // Initialize score

    for (size_t i = 0; i < questions.size(); ++i) {
        cout << "Q" << i + 1 << ": " << questions[i].first << endl;
        const auto &options = questions[i].second.first;
        for (size_t j = 0; j < options.size(); ++j) {
            cout << j + 1 << ". " << options[j] << endl;
        }

        // Correct answer(s) handling
        vector<int> correctAnswers;
        int correct = questions[i].second.second;
        if (correct > 4) { 
            // Extract individual digits as correct options
            while (correct > 0) {
                correctAnswers.push_back(correct % 10);
                correct /= 10;
            }
        } else {
            correctAnswers.push_back(correct + 1); // Convert to 1-based index
        }

        vector<int> currentAnswers; // User's answers for this question
        int attempts;
        if (correctAnswers.size() > 1) {
            while (true) {
                cout << "This is a multiple-correct question. How many attempts would you like to make? ";
                try {
                    cin >> attempts;
                    if (cin.fail() || attempts < 1)
                        throw invalid_argument("Invalid input");
                    break;
                } catch (const invalid_argument&) {
                    cout << "Invalid input. Please enter a valid number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        } else {
            attempts = 1; // Single correct answer means only one attempt
        }

        for (int a = 0; a < attempts; ++a) {
            int choice;
            while (true) {
                cout << "Enter your choice for attempt " << a + 1 << ": ";
                try {
                    cin >> choice;
                    if (cin.fail() || choice < 1 || choice > options.size())
                        throw invalid_argument("Invalid input");
                    break;
                } catch (const invalid_argument&) {
                    cout << "Invalid input. Please enter a valid option number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }

            currentAnswers.push_back(choice);
        }

        userAnswers.push_back(currentAnswers); // Store user's answers

        // Scoring
        int score = 0;
        for (const int &choice : currentAnswers) {
            bool isCorrect = false;
            for (const int &correctOption : correctAnswers) {
                if (choice == correctOption) {
                    isCorrect = true;
                    break;
                }
            }
            score += isCorrect ? 4 : -1; // Award 4 points for correct, deduct 1 for incorrect
        }

        totalScore += score;
    }

    // Write results to file
    string studentFile = "students.txt";
    ofstream file(studentFile, ios::app);

    if (file.is_open()) {
        file << "Quiz: " << quizTitle << endl;
        file << "Answers: ";
        for (const auto &answers : userAnswers) {
            file << "[ ";
            for (const int &answer : answers) {
                file << answer << " ";
            }
            file << "] ";
        }
        file << endl;
        file << "Score: " << totalScore << "/" << questions.size() * 4 << endl;
        file.close();
    } else {
        cout << "Error opening student file for saving data.\n";
    }

    cout << "You scored " << totalScore << " out of " << questions.size() * 4
         << " in the quiz.\n";

    // Save the score in the student's scores map
    scores[quizTitle] = totalScore; 
}

  // Function to view scores
  void viewScores() {
    if (scores.empty()) {
      cout << "You haven't taken any quizzes yet.\n";
    } else {
      cout << "Your Scores:\n";
      for (const auto &scorePair : scores) {
        cout << scorePair.first << ": " << scorePair.second << endl;
      }
    }
  }
};
class Teacher : public User {
private:
  // Quiz data: quiz title -> list of <question, <options, correct option
  // index>> unordered_map<string, vector<pair<string, pair<vector<string>,
  // int>>>> quizzes;
  string subject;                         // Subject taught by the teacher
  unordered_set<string> enrolledStudents; // List of enrolled students

public:
  Teacher(string u, string p, string s) : User(u, p), subject(s) {}

  string getSubject() const { return subject; }

  void menu() override {
    cout << "Teacher Menu (" << subject << ")\n";
    cout << "1. Create Quiz\n";
    cout << "2. View Created Quizzes\n";
    cout << "3. Enroll Student\n";
    cout << "4. View Enrolled Students\n";
    cout << "5. Create Quiz with AI\n"; // New option
    cout << "0. Logout\n";
  }

  void createQuiz() {
    string quizTitle;
    cout << "Enter quiz title: ";
    cin.ignore();
    getline(cin, quizTitle);

    string quizType;
    // Prompt for quiz type until valid input is received
    while (true) {
      cout << "Enter quiz type (SCQ/MCQ): ";
      cin >> quizType;
      cin.ignore(); // Consume newline

      if (quizType == "SCQ" || quizType == "MCQ") {
        break; // Exit the loop if valid input is received
      } else {
        cout << "Invalid quiz type. Please enter SCQ or MCQ.\n";
      }
    }

    vector<pair<string, pair<vector<string>, int>>> questions;
    int numQuestions;
    while (true) {
        cout << "Enter number of questions: ";
        try {
            cin >> numQuestions;
            if (cin.fail()) throw invalid_argument("Invalid input");
            cin.ignore();
            break;
        } catch (const invalid_argument&) {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    for (int i = 0; i < numQuestions; ++i) {
      string question;
      cout << "Enter question " << i + 1 << ": ";
      getline(cin, question);

      vector<string> options;
      // Always 4 options
      for (int j = 0; j < 4; ++j) {
        string option;
        cout << "Enter option " << j + 1 << ": ";
        getline(cin, option);
        options.push_back(option);
      }

      int correctOption;
      if (quizType == "SCQ") {
        while (true) {
            cout << "Enter the correct option number (1-4): ";
            try {
                cin >> correctOption;
                if (cin.fail() || correctOption < 1 || correctOption > 4)
                    throw invalid_argument("Invalid input");
                cin.ignore();
                break;
            } catch (const invalid_argument&) {
                cout << "Invalid input. Please enter a valid option number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
      }
      if (quizType == "MCQ") {

        // Validate input for MCQ

        int number;
        while (true) {
            cout << "Enter the number of correct options: ";
            try {
                cin >> number;
                if (cin.fail() || number < 1 || number > 4)
                    throw invalid_argument("Invalid input");
                cin.ignore();
                break;
            } catch (const invalid_argument&) {
                cout << "Invalid input. Please enter a valid number between 1 and 4.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        for (int i = 0; i < number; i++) {
          int correct;
          while (true) {
              cout << "Enter the correct option number (1-4): ";
              try {
                  cin >> correct;
                  if (cin.fail() || correct < 1 || correct > 4)
                      throw invalid_argument("Invalid input");
                  cin.ignore();
                  break;
              } catch (const invalid_argument&) {
                  cout << "Invalid input. Please enter a valid option number.\n";
                  cin.clear();
                  cin.ignore(numeric_limits<streamsize>::max(), '\n');
              }
          }
          if (i == 0) {
            correctOption = correct;
          } else {
            correctOption *= 10;
            correctOption += correct;
          }
        }
      }

      // Adjust correct option index for vector
      questions.push_back({question, {options, correctOption - 1}});
    }

    quizzes[quizTitle] = questions;
    cout << "Quiz \"" << quizTitle << "\" created successfully.\n";
  }

  void viewQuizzes() {
    if (quizzes.empty()) {
      cout << "No quizzes created yet.\n";
      return;
    }

    for (const auto &quiz : quizzes) {
      cout << "Quiz: " << quiz.first << "\n";
      for (const auto &q : quiz.second) {
        cout << "Q: " << q.first << "\n";
        const auto &options = q.second.first;
        for (size_t i = 0; i < options.size(); ++i) {
          cout << i + 1 << ". " << options[i] << "\n";
        }
        cout << "Correct Option: " << q.second.second + 1 << "\n";
        cout << "-------------------\n";
      }
    }
  }

  void enrollStudent(unordered_map<string, User *> &users) {
    string username;
    cout << "Enter student username to enroll: ";
    cin >> username;

    if (enrolledStudents.find(username) == enrolledStudents.end()) {
      if (users.find(username) != users.end() &&
          dynamic_cast<Student *>(users[username])) {
        enrolledStudents.insert(username);
        Student *student = dynamic_cast<Student *>(users[username]);
        student->addCourse(subject); // Update student's enrolled courses
        cout << "Student " << username << " enrolled successfully.\n";
      } else {
        cout << "Student does not exist.\n";
      }
    } else {
      cout << "Student already enrolled.\n";
    }
  }

  void viewEnrolledStudents() {
    if (enrolledStudents.empty()) {
      cout << "No students enrolled yet.\n";
      return;
    }

    cout << "Enrolled Students:\n";
    for (const auto &student : enrolledStudents) {
      cout << student << "\n";
    }
  }

  void saveEnrolledStudents(ofstream &file) const {
    for (const auto &student : enrolledStudents) {
      file << student << " ";
    }
    file << endl;
  }

  void loadEnrolledStudents(ifstream &file) {
    string student;
    while (file >> student) {
      enrolledStudents.insert(student);
    }
  }

  void createQuizWithAI() {
    string subject;
    int numQuestions;
    cout << "Enter subject for the quiz: ";
    cin.ignore();
    getline(cin, subject);
    cout << "Enter number of questions: ";
    cin >> numQuestions;
    cin.ignore();

    ofstream outfile("quiz_with_ai_input.txt");
    if (!outfile.is_open()) {
      cout << "Error opening file for writing.\n";
      return;
    }

    outfile << subject << endl;
    outfile << numQuestions << endl;
    
    outfile.close();

    Sleep(8000);

    // Read quiz data from "quiz_with_ai_output.txt"
    ifstream infile("quiz_with_ai_output.txt");
    if (!infile.is_open()) {
        cout << "Error opening file for reading.\n";
        return;
    }

    vector<pair<string, pair<vector<string>, int>>> questions;
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue; // Skip empty lines

        string question = line;
        vector<string> options;

        // Read 4 options
        for (int i = 0; i < 4; ++i) {
            if (!getline(infile, line) || line.empty()) {
                cout << "Invalid format in quiz file.\n";
                infile.close();
                return;
            }
            options.push_back(line);
        }

        // Read the correct option
        while (getline(infile, line)) {
            if (line.empty()) continue; // Skip empty lines
            try {
                int correctOption = stoi(line);
                questions.push_back({question, {options, correctOption - 1}});
                break;
            } catch (const invalid_argument &) {
                cout << "Invalid correct option in quiz file.\n";
                infile.close();
                return;
            }
        }
    }
    infile.close();

    // Prompt for quiz title
    string quizTitle;
    cout << "Enter quiz title: ";
    getline(cin, quizTitle);

    // Add the new quiz to the quizzes map
    quizzes[quizTitle] = questions;
    cout << "Quiz \"" << quizTitle << "\" created successfully using AI.\n";
}
};
class Admin : public User {
public:
  Admin(string u, string p) : User(u, p) {}

  void menu() override {
    cout << "Admin Menu\n";
    cout << "1. Manage Users\n";
    cout << "2. Add New Teacher\n";
    cout << "3. Add New Student\n"; // New option added
    cout << "0. Logout\n";          // Add logout option
  }

  void manageUsers() {
    // Existing manage users functionality
  }

  void addTeacher(unordered_map<string, User *> &users) {
    string username, password, subject;
    cout << "Enter new teacher username: ";
    cin >> username;
    cout << "Enter new teacher password: ";
    cin >> password;
    cout << "Enter subject for teacher: ";
    cin >> subject; // New input

    if (users.find(username) == users.end()) {
      users[username] =
          new Teacher(username, password, subject); // Include subject
      cout << "New teacher added successfully.\n";
    } else {
      cout << "User already exists.\n";
    }
  }

  void addStudent(unordered_map<string, User *> &users) {
    string username, password;
    cout << "Enter new student username: ";
    cin >> username;
    cout << "Enter new student password: ";
    cin >> password;

    if (users.find(username) == users.end()) {
      users[username] = new Student(username, password);
      cout << "New student added successfully.\n";
    } else {
      cout << "User already exists.\n"; 
    }
  }
};
class QuizSystem {
private:
  unordered_map<string, User *> users;
  // public:
  // unordered_map<string, vector<pair<string, pair<vector<string>, int>>>>
  // quizzes;
public:
  QuizSystem() {
    // Initialize default users
    // users["student"] = new Student("student", "1234");
    // users["teacher"] = new Teacher("teacher", "1234", "Math"); // Include
    // subject users["admin"] = new Admin("admin", "1234");

    // Load additional users from files
    load_data();
  }

  void load_data() {
    ifstream studentFile("students.txt");
    ifstream teacherFile("teachers.txt");
    ifstream adminFile("admins.txt");

    string username, password, subject;

    while (studentFile >> username >> password) {
      Student *student = new Student(username, password);
      // Load enrolled courses
      string courseLine;
      getline(studentFile, courseLine);
      istringstream courseStream(courseLine);
      string course;
      while (courseStream >> course) {
        student->addCourse(course);
      }
      users[username] = student;
    }

    while (teacherFile >> username >> password >> subject) {
      if (users.find(username) == users.end()) {
        Teacher *teacher = new Teacher(username, password, subject);
        teacher->loadEnrolledStudents(teacherFile); // Load enrolled students
        users[username] = teacher;
      }
    }

    while (adminFile >> username >> password) {
      if (users.find(username) == users.end()) {
        users[username] = new Admin(username, password);
      }
    }

    studentFile.close();
    teacherFile.close();
    adminFile.close();
  }

  ~QuizSystem() {
    for (auto user : users)
      delete user.second;
  }

  void login() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (users.find(username) != users.end() &&
        users[username]->login(username, password)) {
      cout << "Login successful!\n";
      displayMenu(users[username]);
    } else {
      cout << " credentials.\n";
    }
  }

  void displayMenu(User *user) {
    int choice;
    while (true) {
      user->menu();
      while (true) {
          cout << "Enter choice: ";
          try {
              cin >> choice;
              if (cin.fail())
                  throw invalid_argument("Invalid input");
              break;
          } catch (const invalid_argument&) {
              cout << "Invalid input. Please enter a valid number.\n";
              cin.clear();
              cin.ignore(numeric_limits<streamsize>::max(), '\n');
          }
      }
      if (choice == 0) {
        cout << "Logging out...\n";
        break;
      }

      if (Teacher *teacher = dynamic_cast<Teacher *>(user)) {
        if (choice == 1) {
          teacher->createQuiz();
        } else if (choice == 2) {
          teacher->viewQuizzes();
        } else if (choice == 3) {
          teacher->enrollStudent(users); // Pass users map
        } else if (choice == 4) {
          teacher->viewEnrolledStudents(); // New option
        } else if (choice == 5) {
          teacher->createQuizWithAI(); // New option handling
        } else {
          cout << "Invalid choice.\n";
        }
      } else if (Admin *admin = dynamic_cast<Admin *>(user)) {
        if (choice == 1) {
          admin->manageUsers();
        } else if (choice == 2) {
          admin->addTeacher(users);
        } else if (choice == 3) {
          admin->addStudent(users);
        } else {
          cout << "Invalid choice.\n";
        }
      } else if (Student *student = dynamic_cast<Student *>(user)) {
        if (choice == 1) {
          string quizTitle;
          cout << "Enter the quiz title: ";
          cin.ignore();
          getline(cin, quizTitle);
          student->takeQuiz(quizTitle, users);
        } else if (choice == 2) {
          student->viewScores(); // View scores
        } else if (choice == 3) {
          student->viewEnrolledCourses(); // New option
        } else if (choice == 4) {
          string quizTitle;
          cout << "Enter the quiz title to display: ";
          cin.ignore();
          getline(cin, quizTitle);
          student->displayQuiz(quizTitle);
        } else {
          cout << "Invalid choice.\n";
        }
      }
    }
  }

  void save_data() {
    ofstream studentFile("students.txt");
    ofstream teacherFile("teachers.txt");
    ofstream adminFile("admins.txt");

    for (const auto &userPair : users) {
      User *user = userPair.second;
      string username = userPair.first;
      string password = user->getPassword();

      if (Student *student = dynamic_cast<Student *>(user)) {
        studentFile << username << " " << password;
        // Save enrolled courses
        for (const auto &course : student->getEnrolledCourses()) {
          studentFile << " " << course;
        }
        studentFile << endl;
      } else if (Teacher *teacher = dynamic_cast<Teacher *>(user)) {
        teacherFile << username << " " << password << " "
                    << dynamic_cast<Teacher *>(user)->getSubject() << " ";
        dynamic_cast<Teacher *>(user)->saveEnrolledStudents(
            teacherFile); // Save enrolled students
      } else if (dynamic_cast<Admin *>(user)) {
        adminFile << username << " " << password << endl;
      }
    }

    studentFile.close();
    teacherFile.close();
    adminFile.close();
  }
};
int main() {
    load_quizzes();
    QuizSystem system;
    int choice;
    while (true) {
        cout << "-------------------\n";
        cout << "Quiz Management System\n";
        cout << "1. Login\n";
        cout << "2. Exit\n";
        while (true) {
            cout << "Enter choice: ";
            try {
                cin >> choice;
                if (cin.fail())
                    throw invalid_argument("Invalid input");
                break;
            } catch (const invalid_argument&) {
                cout << "Invalid input. Please enter a valid number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        if (choice == 1) {
            system.login();
        } else if (choice == 2) {
            system.save_data();
            save_quizzes();
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}

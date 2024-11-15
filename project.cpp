#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set> // Add this line
#include <fstream>
#include <limits> // Add this line
#include <sstream> // Add this line
using namespace std;

// Base class for a user in the system
class User
{
protected:
    string username;
    string password;

public:
    User(string u, string p) : username(u), password(p) {}

    virtual bool login(string u, string p)
    {
        return username == u && password == p;
    }

    virtual void menu() = 0; // Pure virtual function for role-specific menus

    string getPassword() const { return password; } 
virtual ~User() {}// Added function
// Added function
};

// Student class
class Student : public User
{
private:
    unordered_set<string> enrolledCourses; // New member variable

public:
    Student(string u, string p) : User(u, p) {}

    void menu() override
    {
        cout << "Student Menu\n";
        cout << "1. Take Quiz\n";
        cout << "2. View Scores\n";
        cout << "3. View Enrolled Courses\n"; // New option
        cout << "0. Logout\n";
    }

    void viewEnrolledCourses()
    {
        if (enrolledCourses.empty())
        {
            cout << "You are not enrolled in any courses.\n";
        }
        else
        {
            cout << "Enrolled Courses:\n";
            for (const auto &course : enrolledCourses)
            {
                cout << "- " << course << "\n";
            }
        }
    }

    void addCourse(const string &course)
    {
        enrolledCourses.insert(course);
    }

    const unordered_set<string> &getEnrolledCourses() const
    {
        return enrolledCourses;
    }
};

// Teacher class with ability to create quizzes
class Teacher : public User
{
private:
    unordered_map<string, vector<pair<string, string>>> quizzes; // Quiz title -> list of <question, answer> pairs
    string subject; // New member variable
    unordered_set<string> enrolledStudents; // New member variable

public:
    Teacher(string u, string p, string s) : User(u, p), subject(s) {}

    string getSubject() const { return subject; } // Getter for subject

    void menu() override
    {
        cout << "Teacher Menu (" << subject << ")\n"; // Display subject
        cout << "1. Create Quiz\n";
        cout << "2. View Created Quizzes\n";
        cout << "3. Enroll Student\n"; // Updated option
        cout << "4. View Enrolled Students\n"; // New option
        cout << "0. Logout\n"; // Add logout option
    }

    void createQuiz()
    {
        string quizTitle;
        cout << "Enter quiz title: ";
        cin.ignore();
        getline(cin, quizTitle);

        vector<pair<string, string>> questions;
        int numQuestions;
        cout << "Enter number of questions: ";
        cin >> numQuestions;
        cin.ignore();

        for (int i = 0; i < numQuestions; ++i)
        {
            string question, answer;
            cout << "Enter question " << i + 1 << ": ";
            getline(cin, question);
            cout << "Enter answer for question " << i + 1 << ": ";
            getline(cin, answer);
            questions.push_back({question, answer});
        }

        quizzes[quizTitle] = questions;
        cout << "Quiz \"" << quizTitle << "\" created successfully.\n";
    }

    void viewQuizzes()
    {
        if (quizzes.empty())
        {
            cout << "No quizzes created yet.\n";
            return;
        }
        for (const auto &quiz : quizzes)
        {
            cout << "Quiz: " << quiz.first << "\n";
            for (const auto &qa : quiz.second)
            {
                cout << "Q: " << qa.first << " | A: " << qa.second << "\n";
            }
            cout << "-------------------\n";
        }
    }

    void enrollStudent(unordered_map<string, User*>& users)
    {
        string username;
        cout << "Enter student username to enroll: ";
        cin >> username;

        if (enrolledStudents.find(username) == enrolledStudents.end())
        {
            // Check if the student exists
            if (users.find(username) != users.end() && dynamic_cast<Student*>(users[username]))
            {
                enrolledStudents.insert(username);
                Student* student = dynamic_cast<Student*>(users[username]);
                student->addCourse(subject); // Update student's enrolled courses
                cout << "Student " << username << " enrolled successfully.\n";
            }
            else
            {
                cout << "Student does not exist.\n";
            }
        }
        else
        {
            cout << "Student already enrolled.\n";
        }
    }

    void viewEnrolledStudents()
    {
        if (enrolledStudents.empty())
        {
            cout << "No students enrolled yet.\n";
            return;
        }
        cout << "Enrolled Students:\n";
        for (const auto &student : enrolledStudents)
        {
            cout << student << "\n";
        }
    }

    void saveEnrolledStudents(ofstream &file) const
    {
        for (const auto &student : enrolledStudents)
        {
            file << student << " ";
        }
        file << endl;
    }

    void loadEnrolledStudents(ifstream &file)
    {
        string student;
        while (file >> student)
        {
            enrolledStudents.insert(student);
        }
    }
};

// Admin class with additional admin functionalities
class Admin : public User
{
public:
    Admin(string u, string p) : User(u, p) {}

    void menu() override
    {
        cout << "Admin Menu\n";
        cout << "1. Manage Users\n";
        cout << "2. Add New Teacher\n";
        cout << "3. Add New Student\n"; // New option added
        cout << "0. Logout\n"; // Add logout option
    }

    void manageUsers()
    {
        // Existing manage users functionality
    }

    void addTeacher(unordered_map<string, User *> &users)
    {
        string username, password, subject;
        cout << "Enter new teacher username: ";
        cin >> username;
        cout << "Enter new teacher password: ";
        cin >> password;
        cout << "Enter subject for teacher: ";
        cin >> subject; // New input

        if (users.find(username) == users.end())
        {
            users[username] = new Teacher(username, password, subject); // Include subject
            cout << "New teacher added successfully.\n";
        }
        else
        {
            cout << "User already exists.\n";
        }
    }

    void addStudent(unordered_map<string, User*>& users)
    {
        string username, password;
        cout << "Enter new student username: ";
        cin >> username;
        cout << "Enter new student password: ";
        cin >> password;

        if (users.find(username) == users.end())
        {
            users[username] = new Student(username, password);
            cout << "New student added successfully.\n";
        }
        else
        {
            cout << "User already exists.\n";
        }
    }
};

// Main Quiz System class
class QuizSystem
{
private:
    unordered_map<string, User *> users;

public:
    QuizSystem()
    {
        // Initialize default users
        // users["student"] = new Student("student", "1234");
        // users["teacher"] = new Teacher("teacher", "1234", "Math"); // Include subject
        // users["admin"] = new Admin("admin", "1234");

        // Load additional users from files
        load_data();
    }

    void load_data()
    {
        ifstream studentFile("students.txt");
        ifstream teacherFile("teachers.txt");
        ifstream adminFile("admins.txt");

        string username, password, subject;

        while (studentFile >> username >> password)
        {
            Student* student = new Student(username, password);
            // Load enrolled courses
            string courseLine;
            getline(studentFile, courseLine);
            istringstream courseStream(courseLine);
            string course;
            while (courseStream >> course)
            {
                student->addCourse(course);
            }
            users[username] = student;
        }

        while (teacherFile >> username >> password >> subject)
        {
            if (users.find(username) == users.end())
            {
                Teacher *teacher = new Teacher(username, password, subject);
                teacher->loadEnrolledStudents(teacherFile); // Load enrolled students
                users[username] = teacher;
            }
        }

        while (adminFile >> username >> password)
        {
            if (users.find(username) == users.end())
            {
                users[username] = new Admin(username, password);
            }
        }

        studentFile.close();
        teacherFile.close();
        adminFile.close();
    }

    ~QuizSystem()
    {
        for (auto user : users)
            delete user.second;
    }

    void login()
    {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        if (users.find(username) != users.end() && users[username]->login(username, password))
        {
            cout << "Login successful!\n";
            displayMenu(users[username]);
        }
        else
        {
            cout << " credentials.\n";
        }
    }

    void displayMenu(User* user)
    {
        int choice;
        while (true)
        {
            user->menu();
            cout << "Enter choice: ";
            if (cin >> choice)
            {
                if (choice == 0)
                {
                    cout << "Logging out...\n";
                    break;
                }

                if (Teacher* teacher = dynamic_cast<Teacher*>(user))
                {
                    if (choice == 1)
                    {
                        teacher->createQuiz();
                    }
                    else if (choice == 2)
                    {
                        teacher->viewQuizzes();
                    }
                    else if (choice == 3)
                    {
                        teacher->enrollStudent(users); // Pass users map
                    }
                    else if (choice == 4)
                    {
                        teacher->viewEnrolledStudents(); // New option
                    }
                    else
                    {
                        cout << "Invalid choice.\n";
                    }
                }
                else if (Admin* admin = dynamic_cast<Admin*>(user))
                {
                    if (choice == 1)
                    {
                        admin->manageUsers();
                    }
                    else if (choice == 2)
                    {
                        admin->addTeacher(users);
                    }
                    else if (choice == 3)
                    {
                        admin->addStudent(users);
                    }
                    else
                    {
                        cout << "Invalid choice.\n";
                    }
                }
                else if (Student* student = dynamic_cast<Student*>(user))
                {
                    if (choice == 1)
                    {
                        // Handle student options
                    }
                    else if (choice == 2)
                    {
                        // Handle student options
                    }
                    else if (choice == 3)
                    {
                        student->viewEnrolledCourses(); // New option
                    }
                    else
                    {
                        cout << "Invalid choice.\n";
                    }
                }
            }
            else
            {
                // Handle invalid input
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
            }
        }
    }

    void save_data()
    {
        ofstream studentFile("students.txt");
        ofstream teacherFile("teachers.txt");
        ofstream adminFile("admins.txt");

        for (const auto &userPair : users)
        {
            User *user = userPair.second;
            string username = userPair.first;
            string password = user->getPassword();

            if (Student* student = dynamic_cast<Student*>(user))
            {
                studentFile << username << " " << password;
                // Save enrolled courses
                for (const auto &course : student->getEnrolledCourses())
                {
                    studentFile << " " << course;
                }
                studentFile << endl;
            }
            else if (Teacher* teacher = dynamic_cast<Teacher*>(user))
            {
                teacherFile << username << " " << password << " " << dynamic_cast<Teacher*>(user)->getSubject() << " ";
                dynamic_cast<Teacher*>(user)->saveEnrolledStudents(teacherFile); // Save enrolled students
            }
            else if (dynamic_cast<Admin *>(user))
            {
                adminFile << username << " " << password << endl;
            }
        }

        studentFile.close();
        teacherFile.close();
        adminFile.close();
    }
};

int main()
{
    QuizSystem system;
    int choice;
    while (true)
    {
        cout << "-------------------\n";
        cout << "Quiz Management System\n";
        cout << "1. Login\n";
        cout << "2. Exit\n";
        cout << "Enter choice: ";
        if (cin >> choice)
        {
            if (choice == 1)
            {
                system.login();
            }
            else if (choice == 2)
            {
                system.save_data();
                break;
            }
            else
            {
                cout << "Invalid choice. Try again.\n";
            }
        }
        else
        {
            // Invalid input encountered
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter a number.\n";
        }
    }
    return 0;
}

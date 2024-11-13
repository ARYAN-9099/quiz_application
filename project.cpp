#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
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

    string getPassword() const { return password; } // Added function
};

// Student class
class Student : public User
{
public:
    Student(string u, string p) : User(u, p) {}

    void menu() override
    {
        cout << "Student Menu\n";
        cout << "1. Take Quiz\n";
        cout << "2. View Scores\n";
    }
};

// Teacher class with ability to create quizzes
class Teacher : public User
{
private:
    unordered_map<string, vector<pair<string, string>>> quizzes; // Quiz title -> list of <question, answer> pairs

public:
    Teacher(string u, string p) : User(u, p) {}

    void menu() override
    {
        cout << "Teacher Menu\n";
        cout << "1. Create Quiz\n";
        cout << "2. View Created Quizzes\n";
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
    }

    void manageUsers()
    {
        // Existing manage users functionality
    }

    void addTeacher(unordered_map<string, User *> &users)
    {
        string username, password;
        cout << "Enter new teacher username: ";
        cin >> username;
        cout << "Enter new teacher password: ";
        cin >> password;

        if (users.find(username) == users.end())
        {
            users[username] = new Teacher(username, password);
            cout << "New teacher added successfully.\n";
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
        users["student"] = new Student("student", "1234");
        users["teacher"] = new Teacher("teacher", "1234");
        users["admin"] = new Admin("admin", "1234");

        // Load additional users from files
        load_data();
    }

    void load_data()
    {
        ifstream studentFile("students.txt");
        ifstream teacherFile("teachers.txt");
        ifstream adminFile("admins.txt");

        string username, password;

        while (studentFile >> username >> password)
        {
            if (users.find(username) == users.end())
            {
                users[username] = new Student(username, password);
            }
        }

        while (teacherFile >> username >> password)
        {
            if (users.find(username) == users.end())
            {
                users[username] = new Teacher(username, password);
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
            cout << "Invalid credentials.\n";
        }
    }

    void displayMenu(User *user)
    {
        user->menu();
        int choice;
        cout << "Enter choice: ";
        cin >> choice;

        if (dynamic_cast<Teacher *>(user))
        {
            Teacher *teacher = dynamic_cast<Teacher *>(user);
            if (choice == 1)
            {
                teacher->createQuiz();
            }
            else if (choice == 2)
            {
                teacher->viewQuizzes();
            }
            else
            {
                cout << "Invalid choice.\n";
            }
        }
        else if (dynamic_cast<Student *>(user))
        {
            // Handle student options
        }
        else if (dynamic_cast<Admin *>(user))
        {
            Admin *admin = dynamic_cast<Admin *>(user);
            if (choice == 1)
            {
                admin->manageUsers();
            }
            else if (choice == 2)
            {
                admin->addTeacher(users);
            }
            else
            {
                cout << "Invalid choice.\n";
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

            if (dynamic_cast<Student *>(user))
            {
                studentFile << username << " " << password << endl;
            }
            else if (dynamic_cast<Teacher *>(user))
            {
                teacherFile << username << " " << password << endl;
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
        cin >> choice;

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
    return 0;
}

#code over here
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <stack>
#include <algorithm> // For find_if and sorting
#include <sstream>   // For std::stringstream
#include <iomanip>   // For get_time
#include <unordered_map>

using namespace std;

int taskCounter = 0; // For generating task IDs

struct Task {
    int id;
    string title;
    string description;
    string deadline; // Format: YYYY-MM-DD HH:MM:SS
    int priority;
    string status;
    string reminder; // Format: YYYY-MM-DD HH:MM:SS

    void display() const {
        cout << "ID: " << id << "\nTitle: " << title << "\nDescription: " << description
             << "\nDeadline: " << deadline << "\nPriority: " << priority << "\nStatus: " 
             << status << "\nReminder: " << reminder << "\n";
    }

    // Helper function to convert task to string for writing to file
    string toString() const {
        return to_string(id) + "\n" + title + "\n" + description + "\n" + deadline + "\n" +
            to_string(priority) + "\n" + status + "\n" + reminder + "\n";
    }

    // Helper function to initialize task from string read from file
    static Task fromString(const string& str) {
        Task task;
        stringstream ss(str);
        string line;
        
        getline(ss, line); 
        task.id = stoi(line);
        getline(ss, task.title);
        getline(ss, task.description);
        getline(ss, task.deadline);
        getline(ss, line);
        task.priority = stoi(line);
        getline(ss, task.status);
        getline(ss, task.reminder);
        return task;
    }

    void saveTask(ofstream& file) const {
        file << "Task Data:\n";
        file << "Id: " << id << endl;
        file << "Title: " << title << endl;
        file << "Description: " << description << endl;
        file << "Deadline: " << deadline << endl;
        file << "Priority: " << priority << endl;
        file << "Status: " << status << endl;
        file << "Reminder: " << reminder << endl;
        file << "-----" << endl;
    }
};

vector<Task> tasks;
vector<vector<Task>> taskHistory; // Keeps a history of task states
stack<vector<Task>> redoStack; // Stack for redo operations
stack<unordered_map<int, Task>> snapshotStack; // for snapshots


// Helper functions to handle date-time validation
bool isValidDate(const string& date) {
    struct tm tm = {};
    stringstream ss(date);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Use get_time to parse the date-time string
    if (ss.fail()) { // If get_time fails, it means the date format is incorrect
        return false;
    }
    time_t t = mktime(&tm);
    time_t now = time(0);
    return t >= now; // Ensure the date is not in the past
}

string getCurrentTime() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);
    return string(buffer);
}

string getReminderTime(const string& deadline) {
    struct tm tm = {};
    stringstream ss(deadline);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Parse the deadline string with get_time
    if (ss.fail()) { // If get_time fails, manually return an invalid date
        return "Invalid Date";
    }
    time_t deadlineTime = mktime(&tm);
    deadlineTime -= 24 * 60 * 60; // 24 hours before the deadline
    struct tm* reminderTime = localtime(&deadlineTime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", reminderTime);
    return string(buffer);
}

void loadTasks() {
    ifstream file("tasks.txt");
    string line;
    int maxId = 0;
    
    while (getline(file, line)) {
        if (line.find("Id:") == 0) { // Check if line starts with "Id:"
            istringstream iss(line.substr(3)); // Extract the part after "Id:"
            int id;
            if (iss >> id) {
                maxId = max(maxId, id);
            }
        }
     }
    taskCounter = maxId;
}

void saveTasks(const string& action, const Task& task) {
    ofstream file("tasks.txt", ios::app);  // Open in append mode to add new entries
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    task.saveTask(file);  // Call Task's saveTask method to write details
    file << "----------------------------------" << endl;
    file.close();
}

void createSnapshot() {
    unordered_map<int, Task> snapshot;
    for (const auto& task : tasks) {
        snapshot[task.id] = task;
    }
    snapshotStack.push(snapshot); // Push snapshot onto stack
    
}

void showSnapshots() {
    stack<unordered_map<int, Task>> tempStack = snapshotStack;
    cout << "\nTask Snapshots:\n";

    int snapshotNumber = 1;
    while (!tempStack.empty()) {
        cout << "\nSnapshot " << snapshotNumber++ << ":\n";
        unordered_map<int, Task> snapshot = tempStack.top();
        for (const auto& taskEntry : snapshot) {
            const Task& task = taskEntry.second;
            cout << "\nTask ID: " << task.id
                 << "\nTitle: " << task.title
                 << "\nDescription: " << task.description
                 << "\nDeadline: " << task.deadline
                 << "\nPriority: " << task.priority
                 << "\nStatus: " << task.status
                 << "\nReminder: "<< task.reminder
                 << "\n-------------------------";
        }
        cout << endl;
        tempStack.pop();
    }
}

void createTask() {
    Task newTask;
    int newId = ++taskCounter;
    auto it = find_if(tasks.begin(), tasks.end(), [newId](const Task& task) {
        return task.id == newId;
    });
    if (it != tasks.end()) {
        cout << "Error: Task ID already exists.\n";
        return;
    }

    newTask.id = newId;
    cout << "Enter task title: ";
    getline(cin, newTask.title);
    if (newTask.title.empty()) {
        cout << "Title cannot be empty.\n";
        return;
    }
    cout << "Enter task description: ";
    getline(cin, newTask.description);
    if (newTask.description.empty()) {
        cout << "Description cannot be empty.\n";
        return;
    }
    cout << "Enter deadline (YYYY-MM-DD HH:MM:SS, leave empty for no deadline): ";
    getline(cin, newTask.deadline);
    if (!newTask.deadline.empty()) {
        if (!isValidDate(newTask.deadline)) {
            cout << "Invalid or past deadline. Please enter a valid future date.\n";
            return;
        }
    } else {
        newTask.deadline = "N/A";
    }
    cout << "Enter priority (integer (1 for highest), leave empty for 0): ";
    string priorStr;
    getline(cin, priorStr);
    if (!priorStr.empty()) {
    try {
        newTask.priority = stoi(priorStr);
    } catch (const invalid_argument&) {
        cout << "Invalid priority input. Setting priority to 0." << endl;
        newTask.priority = 0;
    }
} else {
    newTask.priority = 0;
}   

    cout << "Enter status (leave empty for none): ";
    getline(cin, newTask.status);
    if (newTask.status.empty()) {
        newTask.status = "None";
    }
    cout << "Enter reminder time (YYYY-MM-DD HH:MM:SS, leave empty for 24hrs before deadline): ";
    getline(cin, newTask.reminder);
    if (newTask.reminder.empty() && newTask.deadline != "N/A") {
        newTask.reminder = getReminderTime(newTask.deadline);
    }
    tasks.push_back(newTask);
    taskHistory.push_back(tasks); // Save state to history
    saveTasks("Created", newTask); // Save tasks to file
    createSnapshot();
}

void updateTask() {
    int taskId;
    cout << "Enter task ID to update: ";
    cin >> taskId;
    cin.ignore();

    ifstream file("tasks.txt");
    string line;
    bool taskFound = false;
    string updatedTaskData;
    Task task;
    
    while (getline(file, line)) {
        if (line.find("Id:") == 0) { // Check if line starts with "Id:"
            istringstream iss(line.substr(3)); // Extract the part after "Id:"
            int id;
            if (iss >> id && id == taskId) {
                taskFound = true;
                task.id = id;
                updatedTaskData += line + "\n";  // Add the Id line
                break; // Exit loop once the task ID is found
            }
        }
        if (taskFound) {
            updatedTaskData += line + "\n";
        }
    }
    file.close();

    if (taskFound) {
        Task previousState = task; // Save previous state for undo functionality
        
        cout << "Enter new title (leave empty to keep current): ";
        string title;
        getline(cin, title);
        if (!title.empty()) task.title = title;

        cout << "Enter new description (leave empty to keep current): ";
        string description;
        getline(cin, description);
        if (!description.empty()) task.description = description;

        cout << "Enter new deadline (YYYY-MM-DD HH:MM:SS, leave empty to keep current): ";
        string deadline;
        getline(cin, deadline);
        if (!deadline.empty() && isValidDate(deadline)) {
            task.deadline = deadline;
        }
        cout << "Enter new priority (integer, leave empty to keep current): ";
        string priority;
        getline(cin, priority);
        if (!priority.empty()) {
            try {
                task.priority = stoi(priority);
            } catch (const invalid_argument&) {
                cout << "Invalid priority input. Keeping the current priority." << endl;
            }
        }
        cout << "Enter new status (leave empty to keep current): ";
        string status;
        getline(cin, status);
        if (!status.empty()) task.status = status;

        cout << "Enter new reminder time (leave empty to keep current): ";
        string reminder;
        getline(cin, reminder);
        if (!reminder.empty()) task.reminder = reminder;
        taskHistory.push_back(tasks); // Save state to history
        createSnapshot();
        saveTasks("Updated", task); // Save updated tasks to file
        return;
        }
    cout<< "Task ID not found.\n";
}

void deleteTask() {
    int taskId;
    cout << "Enter task ID to delete: ";
    cin >> taskId;
    cin.ignore();

    ifstream file("tasks.txt");
    string line;
    vector<string> fileContents;
    bool taskFound = false;
    bool isTaskData = false;

    while (getline(file, line)) {
        if (line.find("Id:") == 0) { // Check if the line starts with "Id:"
            istringstream iss(line.substr(3)); // Extract the part after "Id:"
            int id;
            if (iss >> id && id == taskId) {
                taskFound = true; 
                isTaskData = true;
                continue; // Skip the current task, effectively deleting it
            }
        }

        if (taskFound && isTaskData) {
            if (line.find("Id:") == 0) {
                isTaskData = false; 
            }
            continue; 
        }
        fileContents.push_back(line);
    }
    file.close();

    if(taskFound){
        ofstream outFile("tasks.txt", ofstream::trunc); // Open the file in truncate mode to overwrite it
        for (const auto& content : fileContents) {
            outFile << content << endl; // Write the remaining lines back to the file
        }
        outFile.close();

        taskHistory.push_back(tasks); // Save state to history
        createSnapshot();
        cout << "Task deleted successfully.\n";
    } else {
        cout << "Task ID not found.\n";
    }
}

// Display tasks in the order they were created
void displayTasksInOrder() {
    cout << "\nTasks in Order of Creation:\n \n";
    ifstream file("tasks.txt");
    if (!file) {
        cerr << "Unable to open the file for reading!" << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
         if (line.empty()) {
            continue;
        }
        cout << line << endl;
        if (line == "-----") continue;
    }
    file.close();
}


// Undo and redo functions as before

void undoTask() {
    if (!taskHistory.empty()) {
        redoStack.push(tasks);
        taskHistory.pop_back();
        if (!taskHistory.empty()) {
            tasks = taskHistory.back(); // Restore previous state
            createSnapshot();
            for(const auto& task : tasks){
                saveTasks("Undone", task);
            }
            cout<<"Undo Successful"<< endl;
        } else {
            cout << "No more actions to undo.\n";
        }
    } else {
        cout << "No more actions to undo.\n";
    }
}

void redoTask() {
    if (!redoStack.empty()) {
        tasks = redoStack.top(); // Restore from redo stack
        taskHistory.push_back(tasks);
        redoStack.pop();
        createSnapshot();
        for (const auto& task : tasks) {
            saveTasks("Redo", task);
        }
        cout << "Redo successful.\n";
    } else {
        cout << "No more actions to redo.\n";
    }
}

int main() {
    loadTasks();  // Load tasks from file
    int choice;
    while (true) {
        cout << "\n1. Create Task\n2. Update Task\n3. Delete Task\n4. Display Tasks\n"
             << "5. Undo\n6. Redo\n7. Snapshots\n8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: createTask(); break;
            case 2: updateTask(); break;
            case 3: deleteTask(); break;
            case 4: displayTasksInOrder(); break;
            case 5: undoTask(); break;
            case 6: redoTask(); break;
            case 7: showSnapshots(); break;
            case 8: return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}

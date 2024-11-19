#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <sqlite3.h>
#include "crow.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann :: json;


struct Task {
    int id;
    string description;
    bool completed;
};

class TaskManager {
private:
    vector<Task> tasks;
    stack<vector<Task>> history; // Stack to store task states for undo
    sqlite3 *db;

public:
    TaskManager() {
        // Open SQLite database connection
        int rc = sqlite3_open("tasks.db", &db);
        if (rc) {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        } else {
            cout << "Opened database successfully" << endl;
        }
        createTable(); // task
        loadTasks(); // load the tasks from db into memory
    }

    ~TaskManager() {
        sqlite3_close(db);
    }

    void createTable() {
        const char *sql = "CREATE TABLE IF NOT EXISTS TASKS("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "DESCRIPTION TEXT NOT NULL,"
                          "COMPLETED INT NOT NULL);";
        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        } else {
            cout << "Table created successfully" << endl;
        }
    }

    void loadTasks() {
        const char *sql = "SELECT * FROM TASKS;";
        sqlite3_stmt *stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to fetch tasks: " << sqlite3_errmsg(db) << endl;
            return;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Task task;
            task.id = sqlite3_column_int(stmt, 0);
            task.description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            task.completed =sqlite3_column_int(stmt, 2);
            tasks.push_back(task);
        }

        sqlite3_finalize(stmt);
    }

    void saveTask(const Task &task) { //new task creation
        const char *sql = "INSERT INTO TASKS (DESCRIPTION, COMPLETED) VALUES (?, ?);";
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, task.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, task.completed);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void updateTaskInDb(const Task &task) {
        const char *sql = "UPDATE TASKS SET DESCRIPTION = ?, COMPLETED = ? WHERE ID = ?;";
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, task.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, task.completed);
        sqlite3_bind_int(stmt, 3, task.id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void deleteTaskFromDb(int taskId) {
        const char *sql = "DELETE FROM TASKS WHERE ID = ?;";
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, taskId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void createTask(const string &description) {
        history.push(tasks);  // Saving current data into history for future undo process
        Task newTask = { static_cast<int>(tasks.size() + 1), description, false };
        tasks.push_back(newTask);
        saveTask(newTask);
    }

    void updateTask(int id, const string &description, bool completed) {
        history.push(tasks);  
        for (auto &task : tasks) {
            if (task.id == id) {
                task.description = description;
                task.completed = completed;
                updateTaskInDb(task);
                break;
            }
        }
    }

    void deleteTask(int id) {
        history.push(tasks);  
        tasks.erase(remove_if(tasks.begin(), tasks.end(), [&](Task &task) { return task.id == id; }), tasks.end());
        deleteTaskFromDb(id);
    }

    vector<Task> getTasks() {
        return tasks;
    }

    bool undo() {
        if (!history.empty()) {
            tasks = history.top();
            history.pop();
            return true;
        }
        return false;
    }
};


int main() {
    TaskManager taskManager;

    
}

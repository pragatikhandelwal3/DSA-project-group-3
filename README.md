#Task Management Application#:



This is a task management application that allows users to create, update, delete, and view tasks. The tasks are stored in a text file and are saved with various details such as title, description, deadline, priority, status, and reminder. The application also supports undo functionality, enabling users to revert changes.

#Features:

*Task Creation: Add new tasks with details such as title, description, deadline, priority, and reminder.
*Task Update: Modify an existing task by updating its title, description, deadline, priority, status, and reminder.
*Task Deletion: Remove tasks from the system by providing the task ID.
*Undo/Redo Functionality: Keep track of task changes with snapshots for easy undo/redo operations.
*Date Validation: Ensure that deadlines and reminders are set correctly with future dates.
*File Persistence: All tasks are stored and retrieved from a text file (tasks.txt).

#Setup(Prequisites):
Make sure you have the following installed:

*C++ Compiler: This project is written in C++. Make sure you have a C++ compiler like GCC or Clang installed on your machine.
*Text Editor/IDE: Use any text editor like VS Code, Sublime Text, or CLion to edit the code.
*Installation Steps:
1)Clone the repository:
git clone https://github.com/pragatikhandelwal3/DSA-project-group-3.git

2)Navigate to the project directory:
cd DSA-project-group-3

3)Compile the code:
g++ -o DSA-project-group-3 main.cpp

4)Run the application:
./DSA-project-group-3

5)Tasks File:
The application uses a file named tasks.txt to store task data. If the file doesn't exist, it will be created automatically upon adding the first task.


#Functionality:

1)Task Creation: To create a task, the application will prompt you to input the following details:

*Title: A brief title for the task.
*Description: A detailed description of what the task entails.
*Deadline: The deadline for the task in the format YYYY-MM-DD HH:MM:SS.
*Priority: An integer representing the priority of the task (e.g., 1 for highest priority).
*Status: The current status of the task (e.g., "In Progress", "Completed").
*Reminder: The reminder time for the task (e.g., 24 hours before the deadline).

2)Task Update: You can update an existing task by providing its ID. The following fields can be updated:

*Title
*Description
*Deadline
*Priority
*Status
*Reminder

3)Task Deletion: To delete a task, provide the task's ID, and the task will be removed from the list.

4)Undo/Redo: The application supports undo and redo functionality using snapshots of task states. This allows you to revert to a previous state of tasks.

5)Viewing Tasks: Tasks can be displayed in the order in which they were created, allowing you to see the progress and changes over time.


#Code Structure

*Task: A struct that holds all task information such as title, description, deadline, and priority.
*taskHistory: A vector storing snapshots of tasks for undo operations.
*redoStack: A stack storing tasks for redo operations.
*snapshotStack: A stack that keeps snapshots of task data for undo/redo functionality.


#File Handling
The tasks are stored in a file named tasks.txt. Each task is written with a "Task Data" header, followed by the task details such as ID, title, description, deadline, priority, and reminder. Each task entry is separated by a line of dashes for easy reading.

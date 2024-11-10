document.addEventListener("DOMContentLoaded", () => {
    const taskList = document.getElementById("tasks");
    const createTaskButton = document.getElementById("create-task");
    const taskModal = document.getElementById("task-modal");
    const taskForm = document.getElementById("task-form");
    const taskTitleInput = document.getElementById("task-title");
    const taskDescriptionInput = document.getElementById("task-description");
    const taskDeadlineInput = document.getElementById("task-deadline");
    const taskPriorityInput = document.getElementById("task-priority");
    const taskStatusInput = document.getElementById("task-status");
    const cancelButton = document.getElementById("cancel");
    const undoButton = document.getElementById("undo");
    const redoButton = document.getElementById("redo");
    const historyButton = document.getElementById("history-btn");
    const searchInput = document.getElementById("search");
    const historyList = document.getElementById("history-list");

    // Store tasks and history
    let tasks = [];
    let history = [];
    let future = [];
    let currentEditIndex = null; // To track which task is being edited

    // Load tasks from localStorage if available
    if (localStorage.getItem("tasks")) {
        tasks = JSON.parse(localStorage.getItem("tasks"));
        renderTasks();
    }

    // Handle the creation of a task
    createTaskButton.addEventListener("click", () => {
        taskModal.style.display = "flex";
        // Clear the input fields for creating a new task
        taskTitleInput.value = "";
        taskDescriptionInput.value = "";
        taskDeadlineInput.value = "";
        taskPriorityInput.value = "1";
        taskStatusInput.value = "In Progress";
        currentEditIndex = null; // Reset edit index when creating a new task
    });

    // Handle task form submission
    taskForm.addEventListener("submit", (e) => {
        e.preventDefault();
        const task = {
            title: taskTitleInput.value,
            description: taskDescriptionInput.value,
            deadline: taskDeadlineInput.value,
            priority: taskPriorityInput.value,
            status: taskStatusInput.value,
        };

        if (task.title) {
            if (currentEditIndex === null) {
                // Create new task
                tasks.push(task);
            } else {
                // Edit existing task
                tasks[currentEditIndex] = task;
            }
            saveState();
            renderTasks();
            taskModal.style.display = "none";
        }
    });

    // Cancel button to close modal
    cancelButton.addEventListener("click", () => {
        taskModal.style.display = "none";
    });

    // Handle task search
    searchInput.addEventListener("input", () => {
        const query = searchInput.value.toLowerCase();
        const filteredTasks = tasks.filter(task => task.title.toLowerCase().includes(query));
        renderTasks(filteredTasks);
    });

    // Handle task rendering
    function renderTasks(taskListArray = tasks) {
        taskList.innerHTML = "";
        taskListArray.forEach((task, index) => {
            const taskItem = document.createElement("li");
            taskItem.innerHTML = `
                <span>${task.title}</span>
                <button class="edit-task" data-index="${index}">Edit</button> 
                <button class="delete-task" data-index="${index}">Delete</button>
            `;
            taskList.appendChild(taskItem);
        });

        // Delete task button
        document.querySelectorAll(".delete-task").forEach((button) => {
            button.addEventListener("click", (e) => {
                const index = e.target.getAttribute("data-index");
                deleteTask(index);
            });
        });

        // Edit task button
        document.querySelectorAll(".edit-task").forEach((button) => {
            button.addEventListener("click", (e) => {
                const index = e.target.getAttribute("data-index");
                editTask(index);
            });
        });
    }

    // Delete a task
    function deleteTask(index) {
        tasks.splice(index, 1);
        saveState();
        renderTasks();
    }

    // Edit a task
    function editTask(index) {
        const task = tasks[index];
        // Populate the modal with the task details
        taskTitleInput.value = task.title;
        taskDescriptionInput.value = task.description;
        taskDeadlineInput.value = task.deadline;
        taskPriorityInput.value = task.priority;
        taskStatusInput.value = task.status;

        taskModal.style.display = "flex"; // Show the modal
        currentEditIndex = index; // Set the current edit index
    }

    // Save current state to localStorage
    function saveState() {
        history.push(JSON.stringify(tasks));
        future = []; // Clear future stack
        localStorage.setItem("tasks", JSON.stringify(tasks));
    }

    // Handle undo action
    undoButton.addEventListener("click", () => {
        if (history.length > 1) {
            future.push(history.pop());
            tasks = JSON.parse(history[history.length - 1]);
            renderTasks();
        }
    });

    // Handle redo action
    redoButton.addEventListener("click", () => {
        if (future.length > 0) {
            const futureState = future.pop();
            history.push(futureState);
            tasks = JSON.parse(futureState);
            renderTasks();
        }
    });

    document.getElementById("display-all-tasks-btn").addEventListener("click", function() {
        const taskListSection = document.getElementById("task-list-section");
      
        // Toggle visibility of the task list section
        if (taskListSection.style.display === "none" || taskListSection.style.display === "") {
          taskListSection.style.display = "block"; // Show the section
        } else {
          taskListSection.style.display = "none"; // Hide the section
        }
      });

    // Handle history page load
    if (window.location.pathname === "/index2.html") {
        renderHistory();
    }

    function renderHistory() {
        historyList.innerHTML = "";
        history.forEach((state, index) => {
            const historyItem = document.createElement("li");
            historyItem.innerHTML = `
                <span>History ${index + 1}</span>
                <button class="restore-history" data-index="${index}">Restore</button>
            `;
            historyList.appendChild(historyItem);
        });

        // Restore task state from history
        document.querySelectorAll(".restore-history").forEach((button) => {
            button.addEventListener("click", (e) => {
                const index = e.target.getAttribute("data-index");
                tasks = JSON.parse(history[index]);
                saveState();
                renderTasks();
            });
        });
    }
});
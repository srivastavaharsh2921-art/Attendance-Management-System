let students = [];
let attendance = [];

function showToast(message, type = "success") {
    let toast = document.getElementById("toast");
    toast.textContent = message;
    toast.className = `show ${type}`;

    setTimeout(() => {
        toast.className = "";
    }, 2200);
}

// Add Student
function addStudent() {
    let id = document.getElementById("studentId").value;
    let name = document.getElementById("studentName").value;

    if (id === "" || name === "") {
        showToast("Please enter ID and Name", "error");
        return;
    }

    students.push({ id, name });
    document.getElementById("studentId").value = "";
    document.getElementById("studentName").value = "";
    showToast("Student added successfully");
}

// Mark Attendance
function markAttendance() {
    let id = document.getElementById("attId").value;
    let status = document.getElementById("status").value;

    let student = students.find(s => s.id == id);

    if (!student) {
        showToast("Student not found", "error");
        return;
    }

    attendance.push({ id, name: student.name, status });
    document.getElementById("attId").value = "";
    showToast("Attendance marked successfully");
}

// Display Records
function displayRecords() {
    let list = document.getElementById("records");
    list.innerHTML = "";

    attendance.forEach(a => {
        let li = document.createElement("li");
        li.className = a.status.toLowerCase();
        li.textContent = `ID: ${a.id} | Name: ${a.name} | Status: ${a.status}`;
        list.appendChild(li);
    });
}

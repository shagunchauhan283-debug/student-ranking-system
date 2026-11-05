# Student Ranking and Result Management System

Advance Data Structures (25CSH-202) · Chandigarh University · B.E CSE Core, Semester 3

A system that stores student records and ranks them by marks without re-sorting everything after each change.

## Data structures used
- **Hash table (separate chaining):** find a student by roll number in O(1) average time
- **Max heap:** the topper is always at the root; add, update and delete take O(log n)

## Features
- Add, update, delete and search students
- Full ranking and top 10 list
- Ties share the same rank (the student added earlier is listed first)
- Input validation (marks 0–100, duplicate and unknown roll numbers)

## Live demo
https://shagunchauhan283-debug.github.io/ranking-system/

## Run the C++ version
```
g++ -std=c++11 main.cpp -o student_ranking
./student_ranking
```

## Files
- `main.cpp`: C++ console application
- `index.html`, `style.css`, `script.js`: web demo of the same logic (HTML, CSS, JavaScript)
- Project report (Word file)

## Team
- [Student 1 Name], UID: [UID 1]
- [Student 2 Name], UID: [UID 2]

Section: 25BCS-705/B · Submitted to: Mr. Anmol Ray (E19861)

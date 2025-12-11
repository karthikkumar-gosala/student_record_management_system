# Student Record Management System 

A role-based Student Management System written in C, featuring login authentication, CRUD operations, grading logic, and table-formatted display. The system uses text files for persistent data storage and supports multiple user roles: Admin, Staff, Student, and Guest.

# Features
# Login System

Users authenticate using credentials stored in credentials.txt.

Each user has one of the following roles:

ADMIN – Full access (Add, Update, Delete, Search, Display)

STAFF – Display, Search, Update Marks

STUDENT – Display, Search

GUEST – View only

# Data Storage
students.txt

Stores student data in the format:

ROLL|NAME|MARKS

credentials.txt

Stores login info:

username password role


# Example:

admin admin123 ADMIN
staff1 staff123 STAFF
student1 stud123 STUDENT
guest guest123 GUEST

# Program Functionalities
# Admin

Add Student

Display All Students

Search Student

Update Student Marks

Delete Student

Logout

# Staff

Display Students

Search Student

Update Marks

Logout

# Student

Display Students

Search Student

Logout

# Guest

Display Students

Logout

# Student Data Fields
Field	Description
Roll Number	Unique identifier
Name	Student name
Marks	Numeric score
CGPA	Auto-calculated (marks / 10)
Grade	Based on marks
Status	PASS/FAIL
# Grading Rules
Marks	Grade	Status
90+	O	PASS
80–89	A+	PASS
70–79	B+	PASS
60–69	C	PASS
45–59	D	PASS
<45	F	FAIL
# Formatted Display Example
ROLL  | NAME          | MARKS | CGPA | GRADE | STATUS
202   | John Carter   | 85.00 | 8.50 | A+    | PASS
103   | Alice Smith   | 42.00 | 4.20 | F     | FAIL
# Sample credentials.txt
admin admin123 ADMIN
staff staff123 STAFF
stud stud123 STUDENT
guest guest123 GUEST

// Student Ranking and Result Management System
// Data structures: Max Heap (ranking) + Hash Table with chaining (record lookup)
// Compile: g++ -std=c++11 student_ranking_system.cpp -o student_ranking
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
using namespace std;
// ---------------------------------------------------------------- Student
struct Student {
    string roll;
    string name;
    double marks;
    int heapIndex;   // position of this student inside the heap
    int order;       // entry order, used to break ties
    Student(string r, string n, double m, int o)
        : roll(r), name(n), marks(m), heapIndex(-1), order(o) {}
};
// true if a should rank above b (higher marks first; on a tie, earlier entry first)
bool better(const Student* a, const Student* b) {
    if (a->marks != b->marks) return a->marks > b->marks;
    return a->order < b->order;
}
// ------------------------------------------------------------- Hash Table
// Separate chaining. Maps roll number -> Student*. The table owns the students.
class HashTable {
    vector<vector<Student*> > buckets;
    int count;
    int hashFn(const string& roll) const {
        unsigned long h = 0;
        for (size_t i = 0; i < roll.size(); i++)
            h = (h * 31 + (unsigned char)roll[i]) % buckets.size();
        return (int)h;
    }
    void resize() {
        vector<vector<Student*> > old = buckets;
        buckets.assign(old.size() * 2 + 1, vector<Student*>());
        for (size_t i = 0; i < old.size(); i++)
            for (size_t j = 0; j < old[i].size(); j++)
                buckets[hashFn(old[i][j]->roll)].push_back(old[i][j]);
    }
public:
    HashTable() : buckets(11), count(0) {}
    ~HashTable() {
        for (size_t i = 0; i < buckets.size(); i++)
            for (size_t j = 0; j < buckets[i].size(); j++)
                delete buckets[i][j];
    }
    bool insert(Student* s) {
        if (search(s->roll) != NULL) return false;           // duplicate roll
        if ((double)count / buckets.size() > 0.75) resize();  // keep load factor low
        buckets[hashFn(s->roll)].push_back(s);
        count++;
        return true;
    }
    Student* search(const string& roll) const {
        const vector<Student*>& b = buckets[hashFn(roll)];
        for (size_t i = 0; i < b.size(); i++)
            if (b[i]->roll == roll) return b[i];
        return NULL;
    }
    // removes from the table and frees the student
    bool remove(const string& roll) {
        vector<Student*>& b = buckets[hashFn(roll)];
        for (size_t i = 0; i < b.size(); i++) {
            if (b[i]->roll == roll) {
                delete b[i];
                b.erase(b.begin() + i);
                count--;
                return true;
            }
        }
        return false;
    }
};
// --------------------------------------------------------------- Max Heap
class MaxHeap {
    vector<Student*> a;
    void swapNodes(int i, int j) {
        swap(a[i], a[j]);
        a[i]->heapIndex = i;
        a[j]->heapIndex = j;
    }
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (better(a[i], a[parent])) {
                swapNodes(i, parent);
                i = parent;
            } else break;
        }
    }
    void siftDown(int i) {
        int n = (int)a.size();
        while (true) {
            int left = 2 * i + 1, right = 2 * i + 2, big = i;
            if (left < n && better(a[left], a[big]))   big = left;
            if (right < n && better(a[right], a[big])) big = right;
            if (big == i) break;
            swapNodes(i, big);
            i = big;
        }
    }
    void removeAt(int i) {
        int last = (int)a.size() - 1;
        if (i != last) swapNodes(i, last);
        a.pop_back();
        if (i < (int)a.size()) {
            siftUp(i);
            siftDown(i);
        }
    }
public:
    int size() const { return (int)a.size(); }
    void insert(Student* s) {
        a.push_back(s);
        s->heapIndex = (int)a.size() - 1;
        siftUp(s->heapIndex);
    }
    Student* extractMax() {
        if (a.empty()) return NULL;
        Student* top = a[0];
        removeAt(0);
        return top;
    }
    void remove(Student* s) { removeAt(s->heapIndex); }
    // call after changing s->marks
    void update(Student* s) {
        siftUp(s->heapIndex);
        siftDown(s->heapIndex);
    }
    // Returns the best k students (as copies) without disturbing the real heap.
    vector<Student> topK(int k) const {
        vector<Student> copies;                 // copy every student, same order
        copies.reserve(a.size());
        for (size_t i = 0; i < a.size(); i++) copies.push_back(*a[i]);
        MaxHeap temp;                           // temp heap with identical shape
        for (size_t i = 0; i < copies.size(); i++) temp.a.push_back(&copies[i]);
        vector<Student> result;
        for (int i = 0; i < k && temp.size() > 0; i++)
            result.push_back(*temp.extractMax());
        return result;
    }
};
// ----------------------------------------------------- Management System
class ResultSystem {
    HashTable table;
    MaxHeap heap;
    int counter;
public:
    ResultSystem() : counter(0) {}
    string add(const string& roll, const string& name, double marks) {
        if (marks < 0 || marks > 100) return "Marks must be between 0 and 100.";
        Student* s = new Student(roll, name, marks, ++counter);
        if (!table.insert(s)) {
            delete s;
            return "Roll number " + roll + " already exists.";
        }
        heap.insert(s);
        return "Student added.";
    }
    string updateMarks(const string& roll, double marks) {
        Student* s = table.search(roll);
        if (!s) return "Student not found.";
        if (marks < 0 || marks > 100) return "Marks must be between 0 and 100.";
        s->marks = marks;
        heap.update(s);
        return "Marks updated.";
    }
    string remove(const string& roll) {
        Student* s = table.search(roll);
        if (!s) return "Student not found.";
        heap.remove(s);      // remove from heap first, then free memory in table
        table.remove(roll);
        return "Student deleted.";
    }
    Student* search(const string& roll) const { return table.search(roll); }
    vector<Student> ranking() const { return heap.topK(heap.size()); }
    vector<Student> top(int k) const { return heap.topK(k); }
};
// ------------------------------------------------------------ Display / UI
void printList(const vector<Student>& list) {
    if (list.empty()) {
        cout << "  (no students)\n";
        return;
    }
    cout << "  " << left << setw(6) << "Rank" << setw(10) << "Roll"
         << setw(20) << "Name" << right << setw(7) << "Marks" << "\n";
    cout << "  " << string(43, '-') << "\n";
    int rank = 0;
    double prev = -1;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].marks != prev) {            // equal marks share the same rank
            rank = (int)i + 1;
            prev = list[i].marks;
        }
        cout << "  " << left << setw(6) << rank << setw(10) << list[i].roll
             << setw(20) << list[i].name << right << setw(7) << list[i].marks << "\n";
    }
}
string readLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}
double readMarks(const string& prompt) {
    string s = readLine(prompt);
    try {
        size_t used;
        double m = stod(s, &used);
        if (used != s.size()) return -1;
        return m;
    } catch (...) {
        return -1;                              // invalid input -> rejected later
    }
}
int main() {
    ResultSystem system;
    while (true) {
        cout << "\n=== Student Ranking & Result Management ===\n"
             << "1. Add student        4. Search student\n"
             << "2. Update marks       5. Show full ranking\n"
             << "3. Delete student     6. Show top 10\n"
             << "0. Exit\n";
        string choice = readLine("Choose: ");
        if (!cin) break;
        if (choice == "1") {
            string roll = readLine("Roll no: ");
            string name = readLine("Name: ");
            double marks = readMarks("Marks (0-100): ");
            cout << system.add(roll, name, marks) << "\n";
        } else if (choice == "2") {
            string roll = readLine("Roll no: ");
            double marks = readMarks("New marks: ");
            cout << system.updateMarks(roll, marks) << "\n";
        } else if (choice == "3") {
            cout << system.remove(readLine("Roll no: ")) << "\n";
        } else if (choice == "4") {
            Student* s = system.search(readLine("Roll no: "));
            if (s) cout << "  " << s->roll << " | " << s->name << " | " << s->marks << "\n";
            else   cout << "Student not found.\n";
        } else if (choice == "5") {
            printList(system.ranking());
        } else if (choice == "6") {
            printList(system.top(10));
        } else if (choice == "0") {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }
    return 0;
}

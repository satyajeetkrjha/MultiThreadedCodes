/*

 n is 4194304 which is 2 ^22 and is large enough
 Non threaded run times
 1558  1560  1564 1578 1581 1582 1599 1629 1657 1713  Median is 1581.5
 Threaded run times
 488 489 490 497 497 502 514 523 526 592 Median is  499.5

  Pls look at attached screenshot where code is run on linux server of operating system class 
 */

//SATYAJEET JHA 7028666251

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
using namespace std;
using namespace chrono;

class Timer {
public:
    system_clock::time_point Begin, End;
    Timer() { Begin = system_clock::now(); }
    ~Timer() {
        End = system_clock::now();
        system_clock::duration Runtime{ End - Begin };
        cout << "Runtime = " << duration_cast<milliseconds>(Runtime).count() << " ms" << "\n";
    }

};




void bitonicmerge(vector<int>& v, int start, int numofelements, int dir,
    int depth) {

    if (numofelements <= 1)
        return;

    int count = numofelements / 2;
    for (int i = start; i < start + count; i++) {
        if ((v[i] > v[i + count] && dir == 1) ||
            (v[i] < v[i + count] && dir == 0)) {
            swap(v[i], v[i + count]);
        }
    }
    if (depth < 2) {
        std::thread T{ bitonicmerge, std::ref(v), start, count, dir, depth + 1 };
        bitonicmerge(v, start + count, count, dir, depth + 1);
        T.join();
    }
    else {
        bitonicmerge(v, start, count, dir, depth + 1);
        bitonicmerge(v, start + count, count, dir, depth + 1);
    }
}

void bitonicsort(vector<int>& v, int start, int numofelements, int dir,
    int depth) {

    
    if (numofelements <= 1)
        return;

    int count = numofelements / 2;
    // Thread only at the first two levels
    if (depth < 2) {
        std::thread T{ bitonicsort, std::ref(v), start, count, 1, depth + 1 };
        bitonicsort(v, start + count, count, 0, depth + 1);
        T.join();
        bitonicmerge(v, start, numofelements, dir, depth);
    }
    else {
        bitonicsort(v, start, count, 1, depth + 1);
        bitonicsort(v, start + count, count, 0, depth + 1);
        bitonicmerge(v, start, numofelements, dir, depth);
    }
}

int main() {
    int n = 4194304;
    vector<int> v;
    for (int i = 1; i <= n; i++) {
        int val = rand() % (1 << 20);
        v.push_back(val);
    }

    
    {
        Timer tm1;
        bitonicsort(v, 0, n, 1, 0); // 1 is ascending while 0 is descending
        
    }

   
    ofstream Out("output1.txt");

    
    for (auto& it : v)
        Out << it << endl;

    Out.close();

    cout << endl;
    
}

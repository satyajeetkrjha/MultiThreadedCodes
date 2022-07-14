

/*


Dependency analysis :
 First we have a distant matrix 
 vector<vector<int>> Dist(Nodes, vector<int>(Nodes, 9999)); 
 and it is populated by simply reading the graph for the first time and for every k 
 which runs upto the total number of nodes ,we calculate the new distance in newDist using values of Dist 
 and once done with  calculation of values ,
 we update Dist = newDist in while loop(Note :used thread to optimize it from O(N^2) to less than that for large graphs.

 Dist matrix is divided into four blocks just as we divide the matrix in case of
 matrix multiplication 
 and we pass each of this block to the four different threads.
 We have another matrix which stores distance 
 vector<vector<int>> newDist(Nodes, vector<int>(Nodes, 9999));
 and this is done to avoid data race condition .
 How ? 

 if (Dist[i1][j1] > Dist[i1][k] + Dist[k][j1]) {
				newDist[i1][j1] = Dist[i1][k] + Dist[k][j1];
				newNext[i1][j1] = next[i1][k];
			}
			else {
				newDist[i1][j1] = Dist[i1][j1];
				newNext[i1][j1] = next[i1][j1];
			}
We read the values from the Dist matrix 
but update in the newDist matrix so reading and writing are happening
in two seperate matrices so no data race .
If dist[r1,c1] depends on dist[r2,c2] and dist[r2,c2] needs to be calculated earlier ,
we already have it calculated in dist matrix while newDist matrix is used for writing values of r1,c2 calculated .
Once we are done with this k'th iteration we just 
do Dist = newDist using four threads and a function called copyDist.

We do the same for calculation of next .For every kth iteration ,we read from Next while upadte in the 
new Next .
So values of Next is used to calculate the possible values and stored in newNext and once
we are done with this k th iteration ,we put it back in Next .
Now Next values of kth iteration will be used to calculate next values of k+1 th iteration .
At last in while loop which runs till total number of nodes 
we do Next = newNext so that the newly calculated Next can again be used for the next iteration of k.

*/





#include <iostream>
#include <vector>
#include <thread>
#include <list>
#include <fstream>
using namespace std;






void floyd(vector <vector <int> >& Dist, vector <vector <int> >& newDist, 
	vector<vector<int>>&next, vector<vector <int>>&newNext,
	int r1, int r2, int c1, int c2, int k) {

	for (int i1 = r1; i1 <= r2; i1++) {
		for (int j1 = c1; j1 <= c2; j1++) {

			
			if (Dist[i1][j1] > Dist[i1][k] + Dist[k][j1]) {
				newDist[i1][j1] = Dist[i1][k] + Dist[k][j1];
				newNext[i1][j1] = next[i1][k];
			}
			else {
				newDist[i1][j1] = Dist[i1][j1];
				newNext[i1][j1] = next[i1][j1];
			}
			
		}
	}
	
}

void copyDist(vector <vector <int> >& Dist, vector <vector <int> >& newDist,
	vector<vector<int>>& next, vector<vector <int>>& newNext,
	int r1, int r2, int c1, int c2) {
	for (int i1 = r1; i1 <= r2; i1++) {
		for (int j1 = c1; j1 <= c2; j1++) {
			Dist[i1][j1] = newDist[i1][j1];
			next[i1][j1] = newNext[i1][j1];
		}
	}
}





int main() {
	//Read in number of nodes and number of edges
	ifstream In("graph.txt");
	int Nodes, Edges;
	In >> Nodes >> Edges;
	int a, b, c;


	//In >> a >> b >> c; do this within a loop


	vector < list<pair<int, int>>> Graph(Nodes);//read in from graph.txt


	vector<vector<int>> Dist(Nodes, vector<int>(Nodes, 9999));
	vector<vector<int>> Next(Nodes, vector<int>(Nodes, -1));
	
	
	//YOu need to initialize diagonal values in Dist and Next


	//Need to read in graph anc construct initial Dist and Next matrices

	for (int i = 0; i < Edges; i++) {
		int u, v, w;
		In >> u;
		In >> v;
		In >> w;
		Graph[u].push_back(make_pair(v, w));
	}

	In.close();
	vector<vector<int>> newDist(Nodes, vector<int>(Nodes, 9999));
	vector<vector<int>> newNext(Nodes, vector<int>(Nodes, -1));

	for (int i = 0; i < Nodes; i++) {
		Dist[i][i] = 0;
		Next[i][i] = i; // next(v,v) should v as on wikipedia
		for (auto it : Graph[i]) {
			Dist[i][it.first] = min(Dist[i][it.first],it.second);
			Next[i][it.first] = it.first; // 1 ->2 weight 5 and 1-> 2 weight 10 but next remains same
			

		}
	}


	//Implement threaded programming



	
	int k = 0;
	int n1 = Nodes;
	int n3 = Nodes;
	while (k < Nodes) {
		thread T1;
		thread T2;
		thread T3;
		thread T4;


		T1 = thread(floyd, ref(Dist), ref(newDist), ref(Next),ref(newNext), 0, n1 / 2 - 1, 0, n3 / 2 - 1, k); // (0,1)
		T2 = thread(floyd, ref(Dist), ref(newDist), ref(Next), ref(newNext), 0, n1 / 2 - 1, n3 / 2, n3 - 1, k); // (2,2)
		T3 = thread(floyd, ref(Dist), ref(newDist), ref(Next), ref(newNext), n1 / 2, n1 - 1, 0, n3 / 2 - 1, k);//(3,3)
		floyd(ref(Dist), ref(newDist), ref(Next), ref(newNext), n1 / 2, n1 - 1, n3 / 2, n3 - 1, k); //(4,4)
		T1.join();
		T2.join();
		T3.join();
		

		T1 = thread(copyDist, ref(Dist), ref(newDist), ref(Next), ref(newNext), 0, n1 / 2 - 1, 0, n3 / 2 - 1);
		T2 = thread(copyDist, ref(Dist), ref(newDist), ref(Next), ref(newNext), 0, n1 / 2 - 1, n3 / 2, n3 - 1);
		T3 = thread(copyDist, ref(Dist), ref(newDist), ref(Next), ref(newNext), n1 / 2, n1 - 1, 0, n3 / 2 - 1);
		copyDist( ref(Dist), ref(newDist), ref(Next), ref(newNext), n1 / 2, n1 - 1, n3 / 2, n3 - 1);

		T1.join();
		T2.join();
		T3.join();
		
		
		k++;
	}

	cout << "Dist Matrix" << endl;
	for (auto& i : Dist) {
		for (auto& j : i) {
			cout << j << " ";
		}
		cout << endl;
	}
	cout << endl;

	//Print results

	cout << "Next Matrix" << endl;

	for (auto& i : Next) {
		for (auto& j : i) {
			cout << j << " ";
		}
		cout << endl;
	}
	cout << endl;


	return 0;
}

/*
Input file:  graph.txt

	Sample input file

	6 //# of nodes in graph
	12 //# of edges in graph
	0 5 16//an edge from node 0 to node 5 with a weight (cost) of 16
	0 4 15
	0 2 4
	0 3 12
	0 1 9
	1 3 3
	1 5 2
	2 3 14
	2 1 3
	3 4 4
	4 5 1
	5 3 2


	Note that it is possible that there are multiple edges from
	a node to another with either identical or different cost.


	Sample output for the above graph.

Dist Matrix

0 9999 8 9999 11 9 7
9999 0 6 8 9 9999
....





Next Matrix
0 -1 3 -1 2 3
-1 1 2 4 3 -1
...




*/






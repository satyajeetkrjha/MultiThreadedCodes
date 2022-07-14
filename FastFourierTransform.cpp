// FastFourierTransformMultithreading.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <math.h>
#include <complex>
#include <list>
#include <numeric>
#include <random>

using namespace std;

const double pi = 3.1415926536;
const complex<double> I(0, 1); // this is iota 
const  int NUM_THREADS = 4;

int reversebits(int num, int base) {
	int val = 0;
	for (int i = 0; i < base; ++i, num >>= 1) {
		val = (val << 1) | (num & 1);
	}
	return val;
}


void threadedbitreversal(vector <complex<double>>& v, vector<complex<double>>& rev, int start, int end) {

	int n = v.size();
	int logN = 0;
	while ((1 << logN) < n) {
		logN++;
	}
	// 0 to 15 so 15 is included by equal to end
	for (int i = start; i <= end; i++) {
		rev[i] = v[reversebits(i, logN)];
	}

}

void threadCreationFunction(vector<complex<double>>& v, vector <complex<double>>& rev, int size) {


	thread T1, T2, T3, T4;
	int i = 0;
	T1 = thread(threadedbitreversal, ref(v), ref(rev), i * (size) / NUM_THREADS, (((i + 1) * size) / NUM_THREADS) - 1);
	T2 = thread(threadedbitreversal, ref(v), ref(rev), ((i + 1) * size) / NUM_THREADS, (((i + 2) * size) / NUM_THREADS) - 1);
	T3 = thread(threadedbitreversal, ref(v), ref(rev), ((i + 2) * size) / NUM_THREADS, (((i + 3) * size) / NUM_THREADS) - 1);
	T4 = thread(threadedbitreversal, ref(v), ref(rev), ((i + 3) * size) / NUM_THREADS, (((i + 4) * size) / NUM_THREADS) - 1);

	T1.join();
	T2.join();
	T3.join();
	T4.join();
}

void threadcalculate(vector<complex<double>>& rev, int steps,
	complex<double>omega, int start, int end, int m2
) {
	complex <double> w = pow(omega, start);
	for (int j = start; j <= end; j++) {
		for (int k = j; k < rev.size(); k += steps) {
			complex <double> a = w * rev[k + m2];
			complex<double> b = rev[k];

			rev[k] = a + b;
			rev[k + m2] = b - a;
		}
		w = w * omega;
	}


}

void threadedsolvehelper(vector<complex<double>>& rev,
	int steps, int m2, complex<double>w, complex<double>omega) {

	thread T1, T2, T3, T4;
	int i = 0;
	T1 = thread(threadcalculate, ref(rev), steps, omega, i * (m2) / NUM_THREADS, (((i + 1) * m2) / NUM_THREADS) - 1, m2);
	T2 = thread(threadcalculate, ref(rev), steps, omega, ((i + 1) * m2) / NUM_THREADS, (((i + 2) * m2) / NUM_THREADS) - 1, m2);
	T3 = thread(threadcalculate, ref(rev), steps, omega, ((i + 2) * m2) / NUM_THREADS, (((i + 3) * m2) / NUM_THREADS) - 1, m2);
	T4 = thread(threadcalculate, ref(rev), steps, omega, ((i + 3) * m2) / NUM_THREADS, (((i + 4) * m2) / NUM_THREADS) - 1, m2);
	T1.join();
	T2.join();
	T3.join();
	T4.join();
}

void parallelfft(vector<complex<double>> v) {
	int n = v.size();
	int logN = 0;
	while ((1 << logN) < n) {
		logN++;
	}
	vector <complex<double>> rev; // 
	rev.resize(n);
	threadCreationFunction(v, rev, n);
	for (int level = 1; level <= logN; level++) {
		int total = 1 << level;
		int m2 = total >> 1;

		complex<double> w(1, 0);
		complex<double> omega = exp(-I * (pi / m2));
        threadedsolvehelper(rev, total, m2, w, omega);
		
	}

	ofstream Out("output_data.txt");


	for (int i = 0; i < rev.size(); i++) {
		Out << rev[i] << endl;
	}
	Out.close();


}



void serialfft(vector<complex<double>> v) {
	int n = v.size();

	// while 2^n is less than n keep going 
	int logN = 0;
	while ((1 << logN) < n) {
		logN++;
	}

	vector <complex<double> > reverse(n);

	for (int i = 0; i < n; i++) {
		reverse[i] = v[reversebits(i, logN)];
	}



	// now reverse vector stores rearranged array 
	// a0 a1 a2 a3 rearranged to
	// a0 a2 a1 a3 

	// 8 has three heights 
	for (int level = 1; level <= logN; level++) {

		int total = 1 << level; // 2 4 8 
		int m2 = total >> 1;


		complex<double> w(1, 0);
		complex<double> omega = exp(-I * (pi / m2));


		for (int j = 0; j < m2; j++) {
			for (int k = j; k < n; k += total) {

				complex <double> a = w * reverse[k + m2];
				complex<double> b = reverse[k];
				//cout << "k and k+half are " << k << " " << k + m2 << endl;
				reverse[k] = a + b;
				reverse[k + m2] = b - a;

			}
			w = w * omega;
		}
	}

	ofstream Out("output_data1.txt");


	for (int i = 0; i < reverse.size(); i++) {
		Out << reverse[i] << endl;
	}
	Out.close();

	

}


int main()
{
	ifstream fin("data.txt");
	vector<complex<double>> v;
	complex <double> x;
	while (fin >> x) {
		v.push_back(x);
	}

	
	parallelfft(v);
	
}


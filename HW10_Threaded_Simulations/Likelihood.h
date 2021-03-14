#ifndef _SIMULATION
#define _SIMULATION
#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<random>
#include<thread>
#include<mutex>

namespace pic10b {
	void set_parameters(std::string&, double&, double&, int&, unsigned&);
	std::vector<int> read_inputs(std::string);
	class Simulation
	{
	   private:
		   std::mutex MUTEX;
		   double likelihood;
		   int trials;
		   int poiss_in(double, double);
		   double exp_out(double);
		   int find_new_pop(int, double, double, double);
		   void evolve_all(int low, int up, size_t* succ, int init, int target, double dT, double eta, double lam);
	   public:
		   Simulation(int);
		   void evolve(int, int, double, double, double, unsigned);
		   double get_likelihood();
	};
}
#endif

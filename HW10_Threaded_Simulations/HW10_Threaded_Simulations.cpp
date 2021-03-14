// HW10_Threaded_Simulations.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Timer.h"
#include "Likelihood.h"

#include<string>
#include<iostream>

int main() {

	constexpr double dT = 1; // yearly time step

	// parameters use can choose/set
	std::string file_name;
	double eta_val = 0, lam_val = 0;
	int num_trials = 0;
	unsigned max_threads = 0;

	// setting them and reading data
	pic10b::set_parameters(file_name, eta_val, lam_val, num_trials, max_threads);
	auto targets = pic10b::read_inputs(file_name);

	if (targets.size() <= 1) { // would only have 0 or 1 data point
		std::cerr << "file lacks sufficient data!" << '\n';
		return -1;
	}
	else { // have at least one year step
		simple_timer::timer<> t;
		for (unsigned num_threads = 1u; num_threads <= max_threads; ++num_threads) {	 // vary number of threads

			pic10b::Simulation s(num_trials); // use given number of trials for accuracy

			t.tick();	// start or reset timer		
			for (size_t i = 0, last = targets.size() - 1; i < last; ++i) { // compare one year and the next				
				int init = targets[i];
				int target = targets[i + 1];

				// this thing needs to be threaded!
				s.evolve(init, target, dT, eta_val, lam_val, num_threads);
			}
			// display likelihood estimates and the times
			std::cout << "Likelihood is " << s.get_likelihood() << '\n';
			std::cout << "It took " << t.tock() << " with " << num_threads << " thread(s).\n";
		}
	}

	return 0;
}
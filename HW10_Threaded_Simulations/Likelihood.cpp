#include "Likelihood.h"
namespace pic10b {
        void set_parameters(std::string& fileName, double& eta_val, double& lam_val, int& num_trials, unsigned& max_threads) {
            std::cout << "Enter values for eta [/yr] and lambda [/yr] : ";
            double eta, lam;
            std::cin >> eta;
            std::cin >> lam;
            eta_val = eta;
            lam_val = lam;

            std::cout << "Enter data file name: ";
            std::cin >> fileName;

            std::cout << "How many particles? ";
            std::cin >> num_trials;

            std::cout << "Enter maximum number of threads to try: ";
            std::cin >> max_threads;

        }
        std::vector<int> read_inputs(std::string fileName) {
            std::vector<int> years;
            std::ifstream in(fileName);
            if (in.good()) {
                std::string line;
                while (std::getline(in, line)) {
                    if (line.size() > 0) {
                        years.push_back(std::stoi(line));
                    }
                }
            }
            else {
                std::cerr << "The file you selected does not exist." << '\n';
            }
            return years;
        }
        
   
        Simulation::Simulation(int num_trials) {
            trials = num_trials;
                trials;
            likelihood = 1.0;
        }

        void Simulation::evolve(int init, int target, double dT, double eta, double lam, unsigned num_threads) {
            size_t successes = 0;
            std::vector<std::thread> threads;
            size_t num_processes = (size_t)num_threads;
            auto chunk = int(trials / num_threads);
            auto current_low = 0;
            for (size_t i = 0; i < num_processes; i++) {
                auto process = [&](int low, int up, size_t* succ, int init, int target, double dT, double eta, double lam) -> void { Simulation::evolve_all(low, up, succ, init, target, dT, eta, lam); };
                threads.emplace_back(process, current_low, current_low + chunk, &successes, init, target, dT, eta, lam);
                current_low += chunk;
            }
            for (size_t i = 0; i < num_processes; i++) {
                threads[i].join();
            }
            likelihood *= (double)successes / (double)trials;
        }


        double Simulation::get_likelihood() {
            return likelihood;
        }

        int Simulation::poiss_in(double eta, double dT) {
            thread_local std::default_random_engine eng{ std::random_device()() };
            std::poisson_distribution<int> p{ eta * dT };
            return p(eng);
        }

        double Simulation::exp_out(double lam) {
            thread_local std::default_random_engine eng{ std::random_device()() };
            std::exponential_distribution<> e{ lam };
            return e(eng);
        }

        int Simulation::find_new_pop(int init, double dT, double eta, double lam) {
            auto num_in = Simulation::poiss_in(eta, dT);
            auto num_leaving = 0;
            for (int i = 1; i <= init; i++) {
                auto val = Simulation::exp_out(lam);
                if (val < dT) {
                    num_leaving++;
                }
            }
            return init + num_in - num_leaving;
        }
        void Simulation::evolve_all(int low, int up, size_t* succ, int init, int target, double dT, double eta, double lam) {
            auto good_trials = 0;
            for (int i = low; i <= up; i++) {
                if (Simulation::find_new_pop(init, dT, eta, lam) == target) {
                    good_trials++;
                }
            }
            Simulation::MUTEX.lock();
            *succ += good_trials;
            Simulation::MUTEX.unlock();
        }
}

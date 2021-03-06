// COMP.CS.310 Concurrency project spring 2021
// https://plus.tuni.fi/comp.cs.310/spring-2021/project3/desc/
// Do make changes to this file, and create more source
// files in this directory if needed.
// Make sure everything is added to course-gitlab before submitting

#include "concur2021.hh"
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <numeric>
#include "threadpool.h"

const unsigned SAMPLES = 1000;
std::mutex m;

// data structure to count how many time we have seen
// a location (binary id in lib)
typedef uint64_t Counter;
std::unordered_map< concur2021::locationID_t, Counter > locCounters;

/**
 * @brief countLocation raises location counters
 * @param id location id
 */
void countLocation( concur2021::locationID_t id )
{
    // save for later use that we have seen the location
    // (in 1st read C++ will initialize value to zero)
    locCounters[ id ] = locCounters[id] + 1;
}

/**
 * @brief printCounters prints all locations and counters where Ahto has been seen
 */
void printCounters() {
    std::cout << "-------------------------------------------------------------" << std::endl;

    // print number of time we have seen a location
    for( auto& id : locCounters ) {
        auto name = concur2021::locationName( id.first );
        std::cout << name << " seen " << id.second << " times." << std::endl;
    }
}

/**
 * @brief WheresAhto calls concur2021 API to detect locations where Ahto has been seen
 */
void WheresAhto() {
    m.lock();
    auto location = concur2021::detect(); // API tells us the location (binary id)
    m.unlock();

    // print out the name and location URL right away:
    auto locname  = concur2021::locationName( location );
    std::cout << "Found at: " << locname << std::endl;
    std::cout << concur2021::locationURL( location ) << std::endl;

    // record the location seen
    m.lock();
    countLocation( location );
    m.unlock();
}

/**
 * @brief main is the starting point of the program
 * @return exit status
 */
int main() {

    // ThreadPool handles task execution in threads.
    // Calling constructor without parameters sets
    // thread number equal to hardware thread contexts.
    // Defaults to 8 if thread contexts are not defined.
    {
        ThreadPool pool;

        // Add tasks to task queue
        for(unsigned i = 0; i < SAMPLES; i++)
        {
            pool.enqueue(WheresAhto);
        }
    }

    // Print result
    printCounters();

    return EXIT_SUCCESS;

}

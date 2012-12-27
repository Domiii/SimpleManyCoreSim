

#include "Processor.hpp"
#include "Task.hpp"
#include "SimConfig.hpp"

#define MAX_CONFIGS 128

using namespace std;

int main()
{
    Processor processor;

    std::vector<Task> tasks;

    // TODO: Load a bunch of task descriptions from file

    // TODO: Load several different configs from file

    // All different configs that we want to run the tasks under
    vector<SimConfig> configs;


    for (vector<SimConfig>::iterator it = configs.begin(); it != configs.end(); ++it)
    {
        // Change config
        GlobalConfig = *it;

        // Run simulation
        processor.StartBatch(tasks);
    }

    return 0;
}
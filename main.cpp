#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <atomic>
#include "puzzle.h"
#include "grid.h"
#include "salesman.h"
#include "searchAlgorithm.h"
//#include "DifferentialHeuristic.h"

using namespace std;

#define SUBOPTIMAL_BOUND 2

enum algorithm {
    astar,
    dijkstra,
    idastar,
    optimistic,
    weightedAstar,
};

typedef struct argStructPuzzle {
    EnvironmentPuzzle<StatePuzzle> *e;
    HeuristicPuzzle *h;
    StatePuzzle *initialState;
    StatePuzzle *goalState;
    vector<StatePuzzle> path;
    algorithm algo;
} argStructPuzzle;

typedef struct argStructGrid {
    EnvironmentGrid *e;
    HeuristicGrid *h;
    StateGrid *initialState;
    StateGrid *goalState;
    vector<StateGrid> path;
    algorithm algo;
} argStructGrid;

typedef struct argStructSales {
    EnvironmentSales *e;
    HeuristicSales *h;
    StateSales *initialState;
    StateSales *goalState;
    vector<StateSales> path;
    algorithm algo;
} argStructSales;

timed_mutex mut;
atomic_bool enter_thread;

chrono::minutes duration = chrono::minutes{100};

bool printPath;

void programPuzzle(string fileName, algorithm algo, int no_of_problems, int &failed, int &problemCount);
void programGrid(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount);
void programSales(algorithm algo, int no_of_problems, int &failed, int &problemCount);
void *runPuzzle(void *arguments);
void *runGrid(void *arguments);
void *runSales(void *arguments);
static void cleanup_handler(void *arg);

void usage(char *programName) {
    cerr << "Usage: " << programName << " <problem_domain> <algorithm> <print_path>"
                                        " <input_file_1> <input_file_2> <no_of_problems>" << endl
            << "where" << endl
            << "problem_domain: \"p\" (for sliding tile puzzle), \"g\" (for 2D grid), or \"s\" (for travelling salesman)" << endl
            << "algorithm: \"1\" for A*, \"2\" for Dijkstra, \"3\" for IDA*, \"4\" for optimistic search, \"5\" for weighted A*" << endl
            << "print_path: either \"y\" (to print each step once the path to the goal is found) or \"n\" (not to print path)" << endl
            << "input_file_1: the problem instance file for sliding tile puzzle; map file for 2D grid  (this argument should be left blank for travelling salesman)" << endl
            << "input_file_2: the problem instance file for 2D grid (this argument should be left blank for sliding tile puzzle and travelling salesman)" << endl
            << "no_of_problems: maximum number of problems to solve" << endl;
}

int main(int argc, char **argv) {

    if (argc != 5 && argc != 6 && argc != 7) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    string domain = argv[1];
    string print = argv[3];
    if (print == "y" || print == "Y") {
        printPath = true;
    } else if (print == "n" || print == "N") {
        printPath = false;
    } else {
        cerr << "Invalid input." << endl;
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int algoNo = atoi(argv[2]);
    algorithm algo;
    switch (algoNo) {
        case 1:
            algo = astar;
            break;
        case 2:
            algo = dijkstra;
            break;
        case 3:
            algo = idastar;
            break;
        case 4:
            algo = optimistic;
            break;
        case 5:
            algo = weightedAstar;
            break;
        default:
            cerr << "Invalid input." << endl;
            usage(argv[0]);
            exit(EXIT_FAILURE);
    }

    int failed = 0;
    int problemCount = 0;

    if (domain == "p" || domain == "P") {
        // Puzzle
        if (argc != 6) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        string fileName = argv[4];
        int no_of_problems = atoi(argv[5]);
        programPuzzle(fileName, algo, no_of_problems, failed, problemCount);
    } else if (domain == "g" || domain == "G") {
        // 2D Grid
        if (argc != 7) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        string mapFileName = argv[4];
        string problemFileName = argv[5];
        int no_of_problems = atoi(argv[6]);
        programGrid(mapFileName, problemFileName, algo, no_of_problems, failed, problemCount);
    } else if (domain == "s" || domain == "S") {
        // travelling salesman
        if (argc != 5) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        int no_of_problems = atoi(argv[4]);
        programSales(algo, no_of_problems, failed, problemCount);
    } else {
        cerr << "Invalid input." << endl;
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    // print number of problems solved
    cout << endl << "Final results:" << endl;
    cout << "Solved " << problemCount - failed << " out of " << problemCount << " problems." << endl;
    return 0;

}

void programPuzzle(string fileName, algorithm algo, int no_of_problems, int &failed, int &problemCount) {
    ifstream inFile(fileName);

    if (inFile.is_open()) {

        // goal state
        int final[16];
        for (int i = 0; i < 16; i++) {
            final[i] = i;
        }

        StatePuzzle goalState(final);

        EnvironmentPuzzle<StatePuzzle> e;

        HeuristicPuzzle astarHeuristic(1);
        HeuristicPuzzle zeroHeuristic(0);

        // read sample number
        int problemNo;

        for (problemCount = 0; problemCount < no_of_problems; problemCount++) {
            if (!(inFile >> problemNo)) {
                // Cannot read
                break;
            };

            cout << endl << "Problem " << problemNo << endl;

            // read sample content
            int init[16];
            for (int &i : init) {
                inFile >> i;
            }

            // solve puzzle
            StatePuzzle initialState(init);

            cout << "Initial state: ";
            initialState.display();
            argStructPuzzle args;
            args.e = &e;
            args.initialState = &initialState;
            args.goalState = &goalState;
            vector<StatePuzzle> path;
            args.path = path;
            args.algo = algo;
            if (algo == dijkstra) {
                args.h = &zeroHeuristic;
            } else {
                args.h = &astarHeuristic;
            }

            enter_thread = false;

            // create thread to run the algorithm for that problem
            pthread_t t;
            void *res;
            int s;
            s = pthread_create(&t, nullptr, runPuzzle, (void *) &args);
            if (s != 0) {
                cerr << "Cannot create thread." << endl;
                exit(EXIT_FAILURE);
            }

            while (!enter_thread) {};

            if (mut.try_lock_for(duration)) {
                // if successfully locked, remove the lock
                mut.unlock();
            }
            s = pthread_cancel(t);
            if (s != 0) {
                cerr << "Cannot cancel thread." << endl;
                exit(EXIT_FAILURE);
            }

            s = pthread_join(t, &res);
            if (s != 0) {
                cerr << "Cannot join thread." << endl;
                exit(EXIT_FAILURE);
            }
            if (res == PTHREAD_CANCELED) {
                // could not solve problem within specified duration
                cout << "Time out." << endl;
                failed++;
            }
        }
        // has finished running a number of problems
    } else {
        cerr << "Cannot open file." << endl;
        exit(EXIT_FAILURE);
    }
}

void programGrid(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount) {
    ifstream mapFile(mapFileName);
    ifstream problemFile(problemFileName);

    if (mapFile.is_open() && problemFile.is_open()) {
        // read map
        vector<int*> map;
        int xMap;
        int yMap;

        // ignore first line of map file
        string line;
        getline(mapFile, line);
        // read 2nd line
        getline(mapFile, line);
        istringstream lineStream(line);
        string ignore;
        getline(lineStream, ignore, ' ');
        lineStream >> yMap;
        // read 3rd line
        getline(mapFile, line);
        lineStream.str(line);
        lineStream.seekg(0);
        getline(lineStream, ignore, ' ');
        lineStream >> xMap;
        // ignore next line
        getline(mapFile, line);

        char coor;
        // read all coordinates
        for (int i = 0; i < yMap; i++) {
            for (int j = 0; j < xMap; j++) {
                mapFile >> coor;
                if (coor != '.' && coor != 'S') {
                    int *block = new int[2];
                    block[0] = j;
                    block[1] = i;
                    map.emplace_back(block);
                }
            }
        }

        EnvironmentGrid e(xMap, yMap, &map);

        HeuristicGrid astarHeuristic(1);
        HeuristicGrid zeroHeuristic(0);

        // read problem file
        // ignore the first line in problem file
        getline(problemFile, line);

        for (problemCount = 0; problemCount < no_of_problems; problemCount++) {
            if (!getline(problemFile, line)) {
                break;
            };
            if (line == "") {
                continue;
            }
            lineStream.str(line);
            lineStream.seekg(0);
            string ignoreStr;
            getline(lineStream, ignoreStr, '.');
            // ignore "map"
            char ignoreChar;
            lineStream >> ignoreChar;
            lineStream >> ignoreChar;
            lineStream >> ignoreChar;
            // ignore dimensions
            int ignoreInt;
            lineStream >> ignoreInt;
            lineStream >> ignoreInt;
            // read start and goal coordinates
            int start[2];
            int goal[2];
            double bestCost;
            lineStream >> start[0];
            lineStream >> start[1];
            lineStream >> goal[0];
            lineStream >> goal[1];
//            // read the last numbers of each line
            lineStream >> bestCost;

            // solve 2D Grid
            StateGrid initialState(start);
            StateGrid goalState(goal);

            cout << endl << "Initial state: ";
            initialState.display();

            cout << "Goal state: ";
            goalState.display();

            cout << "Cost of best path: " << bestCost << endl;

            argStructGrid args;
            args.e = &e;
            args.initialState = &initialState;
            args.goalState = &goalState;

            vector<StateGrid> path;
            args.path = path;
            args.algo = algo;
            if (algo == dijkstra) {
                args.h = &zeroHeuristic;
            } else {
                args.h = &astarHeuristic;
            }

            enter_thread = false;

            // create thread to run the algorithm for that problem
            pthread_t t;
            void *res;
            int s;
            s = pthread_create(&t, nullptr, runGrid, (void *) &args);
            if (s != 0) {
                cerr << "Cannot create thread." << endl;
                exit(EXIT_FAILURE);
            }

            while (!enter_thread) {};

            if (mut.try_lock_for(duration)) {
                // if successfully locked, remove the lock
                mut.unlock();
            }
            s = pthread_cancel(t);
            if (s != 0) {
                cerr << "Cannot cancel thread." << endl;
                exit(EXIT_FAILURE);
            }
            s = pthread_join(t, &res);
            if (s != 0) {
                cerr << "Cannot join thread." << endl;
                exit(EXIT_FAILURE);
            }
            if (res == PTHREAD_CANCELED) {
                // could not solve problem within specified duration
                cout << "Time out." << endl;
                failed++;
            }
        }

        // has finished running a number of problems
        for (auto &block : map) {
            delete[] block;
        }
//        delete astarHeuristic;
//        delete zeroHeuristic;

    } else {
        cerr << "Cannot open file." <<
             endl;
        exit(EXIT_FAILURE);
    }
}

void programSales(algorithm algo, int no_of_problems, int &failed, int &problemCount) {

    for (problemCount = 0; problemCount < no_of_problems; problemCount++) {
        // create problem instance
        EnvironmentSales e;
        HeuristicSales astarHeuristic(1, e);
        HeuristicSales zeroHeuristic(0, e);
        StateSales goalState(-1);

        // solve problem
        StateSales initialState(1);
        cout << endl << "Problem " << problemCount + 1 << endl;
//        cout << "Initial state: ";
//        initialState.display();
        argStructSales args;
        args.e = &e;
        args.initialState = &initialState;
        args.goalState = &goalState;
        vector<StateSales> path;
        args.path = path;
        args.algo = algo;
        if (algo == dijkstra) {
            args.h = &zeroHeuristic;
        } else {
            args.h = &astarHeuristic;
        }

        enter_thread = false;

        // create thread to run the algorithm for that problem
        pthread_t t;
        void *res;
        int s;
        s = pthread_create(&t, nullptr, runSales, (void *) &args);
        if (s != 0) {
            cerr << "Cannot create thread." << endl;
            exit(EXIT_FAILURE);
        }

        while (!enter_thread) {};

        if (mut.try_lock_for(duration)) {
            // if successfully locked, remove the lock
            mut.unlock();
        }
        s = pthread_cancel(t);
        if (s != 0) {
            cerr << "Cannot cancel thread." << endl;
            exit(EXIT_FAILURE);
        }

        s = pthread_join(t, &res);
        if (s != 0) {
            cerr << "Cannot join thread." << endl;
            exit(EXIT_FAILURE);
        }
        if (res == PTHREAD_CANCELED) {
            // could not solve problem within specified duration
            cout << "Time out." << endl;
            failed++;
        }
    }
    // has finished running a number of problems
}

void *runPuzzle(void *arguments) {
    int s;
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    // ensure mutex is unlocked if time out
    pthread_cleanup_push(cleanup_handler, nullptr);

    if (s != 0) {
        cerr << "Cannot set cancel state for thread." << endl;
        exit(EXIT_FAILURE);
    }

    mut.lock();
    enter_thread = true;

    auto start = chrono::steady_clock::now();

    auto *args = (argStructPuzzle*) arguments;
    if (args->algo == astar || args->algo == dijkstra) {
        Astar<StatePuzzle, ActionPuzzle, EnvironmentPuzzle<StatePuzzle>, HeuristicPuzzle> astarPuzzle(*(args->initialState),
                                                                                         *(args->goalState), *(args->h));
        astarPuzzle.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            astarPuzzle.displayPath(args->path, false);
        }
    } else if (args->algo == idastar) {
        IteratedDeepening<StatePuzzle, ActionPuzzle, EnvironmentPuzzle<StatePuzzle>, HeuristicPuzzle> iterPuzzle;
        iterPuzzle.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            iterPuzzle.displayPath(args->path, false);
        }
    } else if (args->algo == optimistic || args->algo == weightedAstar) {
        WeightedAStar<StatePuzzle, ActionPuzzle, EnvironmentPuzzle<StatePuzzle>, HeuristicPuzzle> weightedPuzzle(*(args->initialState), *(args->goalState), *(args->h), SUBOPTIMAL_BOUND, args->algo == optimistic);
        weightedPuzzle.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            weightedPuzzle.displayPath(args->path, false);
        }
    }

    auto end = chrono::steady_clock::now();

    cout << "Solved in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

    mut.unlock();

    pthread_cleanup_pop(0);

    return nullptr;
}

void *runGrid(void *arguments) {
    int s;
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    // ensure mutex is unlocked if time out
    pthread_cleanup_push(cleanup_handler, nullptr);

    if (s != 0) {
        cerr << "Cannot set cancel state for thread." << endl;
        exit(EXIT_FAILURE);
    }

    mut.lock();
    enter_thread = true;

    auto start = chrono::steady_clock::now();

    auto *args = (argStructGrid*) arguments;
    if (args->algo == astar || args->algo == dijkstra) {
        Astar<StateGrid, ActionGrid, EnvironmentGrid, HeuristicGrid> astarGrid(*(args->initialState),
                                                                                    *(args->goalState), *(args->h));
        astarGrid.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            astarGrid.displayPath(args->path, false);
        }
    } else if (args->algo == idastar) {
        IteratedDeepening<StateGrid, ActionGrid, EnvironmentGrid, HeuristicGrid> iterGrid;
        iterGrid.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            iterGrid.displayPath(args->path, false);
        }
    } else if (args->algo == optimistic || args->algo == weightedAstar) {
        WeightedAStar<StateGrid, ActionGrid, EnvironmentGrid, HeuristicGrid> weightedGrid(*(args->initialState), *(args->goalState), *(args->h), SUBOPTIMAL_BOUND, args->algo == optimistic);
        weightedGrid.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            weightedGrid.displayPath(args->path, false);
        }
    }

    auto end = chrono::steady_clock::now();

    cout << "Solved in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

    mut.unlock();

    pthread_cleanup_pop(0);

    return nullptr;
}

void *runSales(void *arguments) {
    int s;
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    // ensure mutex is unlocked if time out
    pthread_cleanup_push(cleanup_handler, nullptr);

    if (s != 0) {
        cerr << "Cannot set cancel state for thread." << endl;
        exit(EXIT_FAILURE);
    }

    mut.lock();
    enter_thread = true;

    auto start = chrono::steady_clock::now();
    auto *args = (argStructSales*) arguments;
    if (args->algo == astar || args->algo == dijkstra) {
        Astar<StateSales, ActionSales, EnvironmentSales, HeuristicSales> astarSales(*(args->initialState),
                                                                                                      *(args->goalState), *(args->h));
        astarSales.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            astarSales.displayPath(args->path, true);
        }
    } else if (args->algo == idastar) {
        IteratedDeepening<StateSales, ActionSales, EnvironmentSales, HeuristicSales> iterSales;
        iterSales.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            iterSales.displayPath(args->path, true);
        }
    } else if (args->algo == optimistic || args->algo == weightedAstar) {
        WeightedAStar<StateSales, ActionSales, EnvironmentSales, HeuristicSales> weightedSales(*(args->initialState), *(args->goalState), *(args->h), SUBOPTIMAL_BOUND, args->algo == optimistic);
        weightedSales.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            weightedSales.displayPath(args->path, true);
        }
    }

    auto end = chrono::steady_clock::now();

    cout << "Solved in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

    mut.unlock();

    pthread_cleanup_pop(0);

    return nullptr;
}

static void cleanup_handler(void *arg) {
    mut.unlock();
}
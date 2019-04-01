#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <atomic>
#include "puzzle.h"
#include "voxel.h"
#include "grid.h"
#include "searchAlgorithm.h"
#include "DifferentialHeuristic.h"

using namespace std;

enum algorithm {
    astar,
    dijkstra,
    dfid,
    idastar,
    nbs
};

typedef struct argStructPuzzle {
    EnvironmentPuzzle<StatePuzzle> *e;
    HeuristicPuzzle *h;
    StatePuzzle *initialState;
    StatePuzzle *goalState;
    vector<StatePuzzle> path;
    algorithm algo;
} argStructPuzzle;

typedef struct argStructVoxel {
    EnvironmentVoxel *e;
    HeuristicVoxel *h;
    StateVoxel *initialState;
    StateVoxel *goalState;
    vector<StateVoxel> path;
    algorithm algo;
} argStructVoxel;

typedef struct argStructGrid {
    EnvironmentGrid *e;
    HeuristicGrid *h;
    StateGrid *initialState;
    StateGrid *goalState;
    vector<StateGrid> path;
    algorithm algo;
} argStructGrid;

timed_mutex mut;
atomic_bool enter_thread;

chrono::minutes duration = chrono::minutes{100};

bool printPath;

void programPuzzle(string fileName, algorithm algo, int no_of_problems, int &failed, int &problemCount);
void programVoxel(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount, string placement, string lookup);
void programGrid(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount, string placement, string lookup);
void *runPuzzle(void *arguments);
void *runVoxel(void *arguments);
void *runGrid(void *arguments);
static void cleanup_handler(void *arg);

void usage(char *programName) {
    cerr << "Usage: " << programName << " <problem_domain> <algorithm> <print_path>"
                                        " <input_file_1> <input_file_2> <no_of_problems> <placement_strategy> <lookup_strategy>" << endl
            << "where" << endl
            << "problem_domain: \"p\" (for sliding tile puzzle), \"v\" (for 3D voxel), or \"g\" (for 2D grid)" << endl
            << "algorithm: \"1\" for A*, \"2\" for Dijkstra, \"3\" for DFID, \"4\" for IDA*, \"5\" for NBS" << endl
            << "print_path: either \"y\" (to print each step once the path to the goal is found) or \"n\" (not to print path)" << endl
            << "input_file_1: the problem instance file for sliding tile puzzle; map file for 3D voxel or 2D grid" << endl
            << "input_file_2: the problem instance file for 3D voxel or 2D grid (this argument should be left blank for sliding tile puzzle)" << endl
            << "no_of_problems: maximum number of problems to solve" << endl
            << "placement_strategy: \"r\" for random, \"f\" for furthest, \"o\" for optimized pivot placement (only for 3D voxel and 2D grid)" << endl
            << "lookup_strategy: \"s\" for single consistent lookup, \"r\" for single random lookup, \"m\" for maximum of all 10 heuristic (only for 3D voxel and 2D grid)" << endl
            << "For puzzle domain, please place the PDB file PDB2.txt in the current directory before running this program." << endl;
}

int main(int argc, char **argv) {

    if (argc != 6 && argc != 9) {
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
            algo = dfid;
            break;
        case 4:
            algo = idastar;
            break;
        case 5:
            algo = nbs;
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
        string fileName = argv[4];
        int no_of_problems = atoi(argv[5]);
        programPuzzle(fileName, algo, no_of_problems, failed, problemCount);
    } else if (domain == "v" || domain == "V") {
        // Voxel
        if (argc != 9) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        string mapFileName = argv[4];
        string problemFileName = argv[5];
        int no_of_problems = atoi(argv[6]);
        string placement = argv[7];
        string lookup = argv[8];
        programVoxel(mapFileName, problemFileName, algo, no_of_problems, failed, problemCount, placement, lookup);
    } else if (domain == "g" || domain == "G") {
        // 2D Grid
        if (argc != 9) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        string mapFileName = argv[4];
        string problemFileName = argv[5];
        int no_of_problems = atoi(argv[6]);
        string placement = argv[7];
        string lookup = argv[8];
        programGrid(mapFileName, problemFileName, algo, no_of_problems, failed, problemCount, placement, lookup);
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
        // Manhattan distance heuristic
//        vector<pair<int, vector<uint8_t>*>> h = {pair<int, vector<uint8_t>*>{0, nullptr}};
        // Manhattan + PDBs heuristic
        ifstream pdb2File("PDB2.txt");
        if (!pdb2File.is_open()) {
            cerr << "Please place the PDB file PDB2.txt in the build folder before running this program." << endl;
            exit(EXIT_FAILURE);
        }
        vector<uint8_t> pdb2(524160);
        int temp;
        for (int i = 0; i < 524160; i++) {
            pdb2File >> temp;
            pdb2[i] = (uint8_t) temp;
        }
        cout << "PDB2 loaded." << endl;
        vector<pair<int, vector<uint8_t>*>> h = {pair<int, vector<uint8_t>*>{0, nullptr}, // Manhattan distance
                                                 pair<int, vector<uint8_t>*>{3, &pdb2}};  // PDB3 heuristic
        // zero heuristic
        vector<pair<int, vector<uint8_t>*>> h0;
        HeuristicPuzzle astarHeuristic(&h);
        HeuristicPuzzle zeroHeuristic(&h0);

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
            if (algo == astar || algo == idastar || algo == nbs) {
                args.h = &astarHeuristic;
            } else {
                args.h = &zeroHeuristic;
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

void programVoxel(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount, string placement, string lookup) {
    ifstream mapFile(mapFileName);
    ifstream problemFile(problemFileName);

    if (mapFile.is_open() && problemFile.is_open()) {
        // read map
        vector<int*> map;
        int xMap;
        int yMap;
        int zMap;

        // read first line of map file
        string line;
        getline(mapFile, line);
        istringstream lineStream(line);
        string ignore;
        getline(lineStream, ignore, ' ');
        lineStream >> xMap;
        lineStream >> yMap;
        lineStream >> zMap;

        // read all blocked coordinates
        while (true) {
            int *block = new int[3];

            if (!(mapFile >> block[0])) {
                break;
            };
            mapFile >> block[1];
            mapFile >> block[2];

            map.emplace_back(block);
        }

        EnvironmentVoxel e(xMap, yMap, zMap, &map);
//        HeuristicVoxel *astarHeuristic = new HeuristicAstarVoxel;
//        HeuristicVoxel *zeroHeuristic = new HeuristicVoxel;

        char lookupChar;
        if (lookup == "s" || lookup == "S") {
            lookupChar = 's';
        } else if (lookup == "r" || lookup == "R") {
            lookupChar = 'r';
        } else {
            lookupChar = 'm';
        }

        vector<vector<double>> h(10, vector<double>((unsigned long) xMap * (unsigned long) yMap * (unsigned long) zMap, DBL_MAX));
        // zero heuristic
        vector<vector<double>> h0;
        auto start = chrono::steady_clock::now();
        DifferentialHeuristic<StateVoxel, ActionVoxel, EnvironmentVoxel> dif;
        int noPivots = 10;
        if (placement == "r" || placement == "R") {
            // Random placement strategy
            dif.random(e, noPivots, h);
        } else if (placement == "f" || placement == "F") {
            // Furthest placement strategy
            dif.furthest(e, noPivots, h);
        } else {
            // Optimized placement strategy
            dif.optimized(e, noPivots, h);
        }

        HeuristicVoxel astarHeuristic(h, lookupChar, xMap, yMap, zMap);
        HeuristicVoxel zeroHeuristic(h0, lookupChar, xMap, yMap, zMap);

        auto end = chrono::steady_clock::now();
        cout << "Differential heuristic built in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

        // read problem file
        // ignore the first two lines in problem file
        getline(problemFile, line);
        getline(problemFile, line);

        for (problemCount = 0; problemCount < no_of_problems; problemCount++) {
            // read start and goal coordinates
            int start[3];
            int goal[3];
            double ignore;
            if (!(problemFile >> start[0])) {
                // Cannot read
                break;
            };
            problemFile >> start[1];
            problemFile >> start[2];
            problemFile >> goal[0];
            problemFile >> goal[1];
            problemFile >> goal[2];
            // read the last 2 numbers of each line
            problemFile >> ignore;
            problemFile >> ignore;

            // solve voxel
            StateVoxel initialState(start);
            StateVoxel goalState(goal);

            cout << endl << "Initial state: ";
            initialState.display();

            cout << "Goal state: ";
            goalState.display();

            argStructVoxel args;
            args.e = &e;
            args.initialState = &initialState;
            args.goalState = &goalState;
            vector<StateVoxel> path;
            args.path = path;
            args.algo = algo;
            if (algo == astar || algo == idastar || algo == nbs) {
                args.h = &astarHeuristic;
            } else {
                args.h = &zeroHeuristic;
            }
            enter_thread = false;

            // create thread to run the algorithm for that problem
            pthread_t t;
            void *res;
            int s;
            s = pthread_create(&t, nullptr, runVoxel, (void *) &args);
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

void programGrid(string mapFileName, string problemFileName, algorithm algo, int no_of_problems, int &failed, int &problemCount, string placement, string lookup) {
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
//        HeuristicGrid *astarHeuristic = new HeuristicAstarGrid;
//        HeuristicGrid *zeroHeuristic = new HeuristicGrid;

        char lookupChar;
        if (lookup == "s" || lookup == "S") {
            lookupChar = 's';
        } else if (lookup == "r" || lookup == "R") {
            lookupChar = 'r';
        } else {
            lookupChar = 'm';
        }

        vector<vector<double>> h(10, vector<double>((unsigned long) xMap * (unsigned long) yMap, DBL_MAX));
        // zero heuristic
        vector<vector<double>> h0;
        auto start = chrono::steady_clock::now();
        DifferentialHeuristic<StateGrid, ActionGrid, EnvironmentGrid> dif;
        int noPivots = 10;
        if (placement == "r" || placement == "R") {
            // Random placement strategy
            dif.random(e, noPivots, h);
        } else if (placement == "f" || placement == "F") {
            // Furthest placement strategy
            dif.furthest(e, noPivots, h);
        } else {
            // Optimized placement strategy
            dif.optimized(e, noPivots, h);
        }

        HeuristicGrid astarHeuristic(h, lookupChar, xMap, yMap);
        HeuristicGrid zeroHeuristic(h0, lookupChar, xMap, yMap);

        auto end = chrono::steady_clock::now();
        cout << "Differential heuristic built in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

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
            if (algo == astar || algo == idastar || algo == nbs) {
                args.h = &astarHeuristic;
            } else {
                args.h = &zeroHeuristic;
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
            astarPuzzle.displayPath(args->path);
        }
    } else if (args->algo == nbs) {
        NBS<StatePuzzle, ActionPuzzle, EnvironmentPuzzle<StatePuzzle>, HeuristicPuzzle> nbsPuzzle(*(args->initialState),
                                                                                         *(args->goalState), *(args->h));
        nbsPuzzle.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            nbsPuzzle.displayPath(args->path);
        }
    } else if (args->algo == dfid || args->algo == idastar) {
        IteratedDeepening<StatePuzzle, ActionPuzzle, EnvironmentPuzzle<StatePuzzle>, HeuristicPuzzle> iterPuzzle;
        iterPuzzle.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path, args->algo == dfid);
        if (printPath) {
            cout << "Steps:" << endl;
            iterPuzzle.displayPath(args->path);
        }
    }

    auto end = chrono::steady_clock::now();

    cout << "Solved in " << chrono::duration_cast<chrono::seconds>(end - start).count() << "s." << endl;

    mut.unlock();

    pthread_cleanup_pop(0);

    return nullptr;
}

void *runVoxel(void *arguments) {
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

    auto *args = (argStructVoxel*) arguments;
    if (args->algo == astar || args->algo == dijkstra) {
        Astar<StateVoxel, ActionVoxel, EnvironmentVoxel, HeuristicVoxel> astarVoxel(*(args->initialState),
                                                                                         *(args->goalState), *(args->h));
        astarVoxel.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            astarVoxel.displayPath(args->path);
        }
    } else if (args->algo == nbs) {
        NBS<StateVoxel, ActionVoxel, EnvironmentVoxel, HeuristicVoxel> nbsVoxel(*(args->initialState),
                                                                                     *(args->goalState), *(args->h));
        nbsVoxel.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
        if (printPath) {
            cout << "Steps:" << endl;
            nbsVoxel.displayPath(args->path);
        }
    } else if (args->algo == dfid || args->algo == idastar) {
        IteratedDeepening<StateVoxel, ActionVoxel, EnvironmentVoxel, HeuristicVoxel> iterVoxel;
        iterVoxel.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path, args->algo == dfid);
        if (printPath) {
            cout << "Steps:" << endl;
            iterVoxel.displayPath(args->path);
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
                astarGrid.displayPath(args->path);
            }
        } else if (args->algo == nbs) {
            NBS<StateGrid, ActionGrid, EnvironmentGrid, HeuristicGrid> nbsGrid(*(args->initialState),
                                                                                    *(args->goalState), *(args->h));
            nbsGrid.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path);
            if (printPath) {
                cout << "Steps:" << endl;
                nbsGrid.displayPath(args->path);
            }
        } else if (args->algo == dfid || args->algo == idastar) {
            IteratedDeepening<StateGrid, ActionGrid, EnvironmentGrid, HeuristicGrid> iterGrid;
            iterGrid.getPath(*(args->e), *(args->h), *(args->initialState), *(args->goalState), args->path, args->algo == dfid);
            if (printPath) {
                cout << "Steps:" << endl;
                iterGrid.displayPath(args->path);
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
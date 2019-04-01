How to build the program:
    cmake -S <path-to-source> -B <path-to-build>
    cd <path-to-build>
    make



How to run the code:
    ./assignment4 <problem_domain> <algorithm> <print_path> <input_file_1> <input_file_2> <no_of_problems> <placement_strategy> <lookup_strategy>
    where
        problem_domain: "p" (for sliding tile puzzle), "v" (for 3D voxel), or "g" (for 2D grid)
        algorithm: "1" for A*, "2" for Dijkstra, "3" for DFID, "4" for IDA*, "5" for NBS
        print_path: either "y" (to print each step once the path to the goal is found) or "n" (not to print path)
        input_file_1: the problem instance file for sliding tile puzzle; map file for 3D voxel or 2D grid
        input_file_2: the problem instance file for 3D voxel or 2D grid (this argument should be left blank for sliding tile puzzle)
        no_of_problems: maximum number of problems to solve
        placement_strategy: "r" for random, "f" for furthest, "o" for optimized pivot placement (only for 3D voxel and 2D grid)
        lookup_strategy: "s" for single consistent lookup, "r" for single random lookup, "m" for maximum of all 10 heuristic (only for 3D voxel and 2D grid)

    For puzzle domain, please place the PDB files PDB1.txt, PDB2.txt, PDB3.txt in the current directory before running this program.
    These 3 PDB files have been pre-built using the PDB builder in part 1.
    PDB1.txt: tiles 0-7
    PDB2.txt: tiles 0,8-9,12-13
    PBB3.txt: tiles 0,10-11,14-15
    The maximum of Manhattan distance and the heuristic values from the 3 PDBs will be taken as the heuristic for the puzzle domain.



How to use the parsing program to analyze the output of the solver to get a summary:
    ./parsing <output_file>
    where
        output_file: name of the output file of the solver program
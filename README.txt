How to build the program:
    cmake -S <path-to-source> -B <path-to-build>
    cd <path-to-build>
    make



How to run the code:
    ./project <problem_domain> <algorithm> <print_path> <input_file_1> <input_file_2> <no_of_problems> <placement_strategy> <lookup_strategy>
    where
        problem_domain: "p" (for sliding tile puzzle), "g" (for 2D grid), or "s" (for travelling salesman)
        algorithm: "1" for A*, "2" for IDA*, "3" for optimistic search, "4" for weighted A*
        print_path: either "y" (to print each step once the path to the goal is found) or "n" (not to print path)
        input_file_1: the problem instance file for sliding tile puzzle; map file for 2D grid (this argument should be left blank for travelling salesman)
        input_file_2: the problem instance file for 2D grid (this argument should be left blank for sliding tile puzzle and travelling salesman)
        no_of_problems: maximum number of problems to solve


How to use the parsing program to analyze the output of the solver to get a summary:
    ./parsing <output_file>
    where
        output_file: name of the output file of the solver program
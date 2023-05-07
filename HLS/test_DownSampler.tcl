# Configuration
set cflags "-std=c++11"

# Project
open_project -reset "DownSampler"
set_top AverageDownSampler
add_files -tb DownSampler.cpp -cflags "${cflags}"

# Solution
open_solution -reset "solution"
set_part {xcvu9p-flga2104-2L-e}
create_clock -period 3.0 -name default

# Run C simulation
puts "***** C SIMULATION *****"
csim_design

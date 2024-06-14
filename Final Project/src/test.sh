#!/bin/bash
./atpg -tdfatpg -ndet $2 ../sample_circuits/c$1.ckt > ../tdf_patterns/nieh_c17.pat
grep "cpu" ../tdf_patterns/nieh_c17.pat
grep "vector" ../tdf_patterns/nieh_c17.pat
grep "fault coverage" ../tdf_patterns/nieh_c17.pat
grep "uncollapse" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $2 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c$1.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log

#!/bin/bash

echo "For $1-det."
if [ $# -ne 1 ]; then
	echo "Compression turns on."
	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c432.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C432:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c432.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c499.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C499:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c499.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c880.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C880:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c880.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c1355.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C1355:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c1355.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c2670.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C2670:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c2670.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c3540.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C3540:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c3540.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat

	./atpg -tdfatpg -ndet $1 -compression ../sample_circuits/c7552.ckt > ../tdf_patterns/nieh_c17.pat
	echo "C7552:"
	grep "vector" ../tdf_patterns/nieh_c17.pat
	./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c7552.ckt > ../tdf_patterns/sim.log
	grep "fault coverage" ../tdf_patterns/sim.log
	grep "generation" ../tdf_patterns/nieh_c17.pat
	exit 1
fi

echo "Compression turns off."
./atpg -tdfatpg -ndet $1 ../sample_circuits/c432.ckt > ../tdf_patterns/nieh_c17.pat
echo "C432:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c432.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c499.ckt > ../tdf_patterns/nieh_c17.pat
echo "C499:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c499.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c880.ckt > ../tdf_patterns/nieh_c17.pat
echo "C880:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c880.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c1355.ckt > ../tdf_patterns/nieh_c17.pat
echo "C1355:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c1355.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c2670.ckt > ../tdf_patterns/nieh_c17.pat
echo "C2670:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c2670.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c3540.ckt > ../tdf_patterns/nieh_c17.pat
echo "C3540:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c3540.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

./atpg -tdfatpg -ndet $1 ../sample_circuits/c7552.ckt > ../tdf_patterns/nieh_c17.pat
echo "C7552:"
grep "vector" ../tdf_patterns/nieh_c17.pat
./atpg -ndet $1 -tdfsim ../tdf_patterns/nieh_c17.pat ../sample_circuits/c7552.ckt > ../tdf_patterns/sim.log
grep "fault coverage" ../tdf_patterns/sim.log
grep "generation" ../tdf_patterns/nieh_c17.pat

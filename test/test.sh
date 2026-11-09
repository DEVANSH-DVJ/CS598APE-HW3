#!/bin/bash
# Run one benchmark on one built commit REPS times and print the times in seconds.
#   test/test.sh base 1001
# Benchmarks: grid sizes, such as 1001.
# The image is saved as test/results/<name>-<benchmark>.png.
# Every time is also added to test/results/times.csv.
# Environment: REPS (default 3), THREADS (default 1)
cd "$(dirname "$0")/.."
name=$1
bench=$2
mkdir -p test/results
out=$PWD/test/results/$name-$bench.png
csv=$PWD/test/results/times.csv
[ -f $csv ] || echo "commit,benchmark,threads,seconds" > $csv

# Time is process wall-clock seconds
TIMEFORMAT=%R

cd test/build/$name
echo -n "$name $bench:"
for r in $(seq ${REPS:-3}); do
  t=$( { time OMP_NUM_THREADS=${THREADS:-1} ./sandpile.exe $bench > run.log 2>&1; } 2>&1 )
  if [ $? -ne 0 ]; then rm -f $out; echo " failed"; exit 1; fi
  mv identity.png $out
  echo -n " $t"
  echo "$name,$bench,${THREADS:-1},$t" >> $csv
done
echo

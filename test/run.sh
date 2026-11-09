#!/bin/bash
# Build each commit and run the benchmark on it.
#   test/run.sh              all commits in test/commits.txt
#   test/run.sh base opt1    only these
cd "$(dirname "$0")/.."
names=${*:-$(grep -v '^#' test/commits.txt | cut -d' ' -f1)}

for name in $names; do
  test/build.sh $name
  for bench in 1001; do
    test/test.sh $name $bench
  done
done

#!/bin/bash
# Build one commit from test/commits.txt into test/build/<name>.
#   test/build.sh base
cd "$(dirname "$0")/.."
name=$1
sha=$(grep "^$name " test/commits.txt | awk '{print $2}')
dir=test/build/$name

rm -rf $dir
mkdir -p $dir
git archive $sha | tar -x -C $dir
make -C $dir -j > $dir/build.log 2>&1 || echo "$name: build failed, see $dir/build.log"

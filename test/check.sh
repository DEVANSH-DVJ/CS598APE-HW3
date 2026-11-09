#!/bin/bash
# After test/run.sh: check that every commit drew the same image as the one before it and identity-ref.png.
# Images must be pixel-identical; a difference shows the number of changed pixels.
cd "$(dirname "$0")/results"
names=$(grep -v '^#' ../commits.txt | cut -d' ' -f1)

# Compare the current image with one reference
check() {
  if [ ! -f $name-$bench.png ] || [ ! -f $2 ]; then
    echo "$name $bench: no results"
    return
  fi
  # First field is the changed pixel count
  ae=$(compare -metric AE $name-$bench.png $2 null: 2>&1 | cut -d' ' -f1)
  if [ "$ae" = 0 ]; then
    echo "$name $bench: same as $1"
  else
    echo "$name $bench: DIFFERENT from $1 ($ae pixels)"
  fi
}

prev=
for name in $names; do
  for bench in 1001; do
    if [ -n "$prev" ]; then
      check $prev $prev-$bench.png
    fi
    check identity-ref ../../identity-ref.png
  done
  prev=$name
done

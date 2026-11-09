# 598APE-HW3: optimized abelian sandpile

This fork speeds up the course abelian sandpile benchmark with 8 optimizations, one commit each.
No optimization needs special hardware.
The original course README follows below.

- **Tools:**
  everything runs in the container built from `docker/Dockerfile`, an Ubuntu 24.04 image with GCC 13.3, Clang 19, ImageMagick, clang-format, and matplotlib.
  `./dockerrun.sh` builds the image on first use and opens a shell with this repository at `/host`, where the commands below run.
- **Build and run:**
  `make all`, then `./sandpile.exe <N>`, which writes `identity.png`.
  `OMP_NUM_THREADS` sets the thread count.
- **Reproduce:**
  `test/run.sh` builds every commit below and times `./sandpile.exe 1001` on it, three runs each on one thread, and `test/check.sh` checks that every commit draws the same image as the one before it and as `identity-ref.png`.
  The image is the same at every commit and every thread count.
  For the times at 16 threads we ran `taskset -c 0-15 env THREADS=16 OMP_PROC_BIND=close OMP_PLACES=cores test/run.sh opt6`, and likewise for the other commits and thread counts.
- **Figures:**
  `python3 plots/scripts/plot.py` draws the figures of the report from `plots/data/times.csv`.

| Name   | Commit    | Change                                    | Report |
| ------ | --------- | ----------------------------------------- | ------ |
| `base` | `15fe837` | unmodified course code                    |        |
| `opt1` | `025019f` | precompute each row's Pacman columns      | 2.1    |
| `opt2` | `8a189d7` | use one padded grid with a local stride   | 2.1    |
| `opt3` | `18638f5` | topple each row at once, vectorized       | 2.2    |
| `opt4` | `b697838` | store cells as 16-bit unsigned shorts     | 2.3    |
| `opt5` | `7b8eb0a` | topple byte cells, flush to the int grid  | 2.3    |
| `opt6` | `234356c` | topple rows of one color in parallel      | 2.4    |
| `opt7` | `da678b2` | keep one thread team per stabilize        | 2.4    |
| `opt8` | `a23393b` | topple half the rows of odd grids         | 2.5    |

---

# CS598APE HW3

This repository contains code for homework 3 of CS598APE. It is relatively
simple in comparison to HW1 and HW2 to ensure you have enough time to work on
the course project. 

`sandpile.c` contains the implementation of finding the identity element of a 2D
abelian sandpile.

![Identity sandpile on a 1001x1001 pacman grid](identity-ref.png)

*Identity sandpile on a 1001x1001 pacman grid*


To compile the program, run:

```bash
make all
```

To clean existing build artifacts, run:

```bash
make clean
```

Usage (after compiling):

```bash
./sandpile.exe <N> 
```

where `N` is the grid size

Consider speeding up the generation of this 1001x1001 identity sandpile (which
takes approximately ~3m 40s on my local laptop under the default setup)

```bash
./sandpile.exe 1001
```

For an optional, ungraded challenge, see how large a sandpile you can compute
on the course VMs and report the grid size and runtime. There is no prescribed
upper limit on `N` for this challenge.


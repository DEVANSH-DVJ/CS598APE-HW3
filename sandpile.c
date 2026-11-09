#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static void die(const char *s) {
  fprintf(stderr, "%s\n", s);
  exit(1);
}

int N = 801;
// One block with a one-cell border, row stride N + 2
int *grid;
// Cell (i, j) inside the border
#define CELL(i, j) grid[((i) + 1) * (N + 2) + (j) + 1]
// Pacman column range of each row
int *lo, *hi;

// you can also use this function to debug print your sandpile
static void render(char *filename) {
  int chan = 3;
  unsigned char *pixels = (unsigned char *)malloc(N * N * chan * sizeof(char));
  int stride = N * chan;
  if (!pixels)
    die("malloc failed for PNG pixel buffer");
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      unsigned char r = 0, g = 0, b = 0;
      switch (CELL(i, j)) {
      case 0: // obsidian
        r = 15;
        g = 15;
        b = 25;
        break;
      case 1: // electric cyan
        r = 0;
        g = 210;
        b = 255;
        break;
      case 2: // vibrant magenta
        r = 230;
        g = 30;
        b = 130;
        break;
      case 3: // bright gold
        r = 255;
        g = 200;
        b = 0;
        break;
      default: // white
        r = 0;
        g = 0;
        b = 0;
      }

      int pid = (i * N + j) * chan;
      pixels[pid] = r;
      pixels[pid + 1] = g;
      pixels[pid + 2] = b;
    }
  }
  if (stbi_write_png(filename, N, N, chan, pixels, stride)) {
    printf("Successfully saved sandpile identity to: %s\n", filename);
  } else {
    die("Error: stb_image_write failed to save png file\n");
  }
  free(pixels);
}

// Pacman cells of a row form one interval: a disk row cut by the mouth
static void find_ranges() {
  lo = (int *)malloc(N * sizeof(int));
  hi = (int *)malloc(N * sizeof(int));
  for (int i = 0; i < N; ++i) {
    // Empty range until a cell is found
    lo[i] = N;
    hi[i] = -1;
    for (int j = 0; j <= MAX(i, N - 1 - i); ++j) {
      if ((i - N / 2) * (i - N / 2) + (j - N / 2) * (j - N / 2) <= (N / 2) * (N / 2)) {
        lo[i] = MIN(lo[i], j);
        hi[i] = j;
      }
    }
  }
}

// Moves byte grains in a to b outside l to h to the int grid
static void flush(unsigned char *c, int *s, int a, int b, int l, int h) {
  for (int j = a; j <= MIN(b, l - 1); ++j) {
    s[j] += c[j];
    c[j] = 0;
  }
  for (int j = MAX(a, h + 1); j <= b; ++j) {
    s[j] += c[j];
    c[j] = 0;
  }
}

// Topples whole rows at once: each Pacman cell v topples v / 4 times
static void stabilize() {
  // Locals, so grid stores cannot alias them
  int n = N, w = N + 2;
  // Byte copy of the Pacman cells, at most 11
  unsigned char *cells = (unsigned char *)calloc((size_t)w * w, 1);
  for (int i = 0; i < n; ++i)
    for (int j = lo[i]; j <= hi[i]; ++j)
      cells[(i + 1) * w + j + 1] = CELL(i, j);
  // Topple counts of one row, columns -1 to n
  unsigned char *restrict top = (unsigned char *)calloc(w, 1) + 1;
  int changed = 0;
  do {
    changed = 0;
    for (int i = 0; i < n; ++i) {
      unsigned char *restrict row = cells + (i + 1) * w + 1;
      unsigned char *restrict up = row - w;
      unsigned char *restrict down = row + w;
      int *out = grid + (i + 1) * w + 1;
      // Visit only the row's Pacman range
      int a = lo[i], b = hi[i];
      unsigned char any = 0;
      for (int j = a; j <= b; ++j) {
        top[j] = row[j] >> 2;
        any |= top[j];
      }
      // Cells beside the range never topple
      top[a - 1] = 0;
      top[b + 1] = 0;
      // Pacman cells are never negative, so v - 4 (v / 4) is v & 3
      for (int j = a; j <= b; ++j) {
        row[j] = (row[j] & 3) + top[j - 1] + top[j + 1];
        up[j] += top[j];
        down[j] += top[j];
      }
      // Grains off the range or grid land in the int grid
      out[a - 1] += top[a];
      out[b + 1] += top[b];
      // Border rows hold no Pacman cells, so every column moves to the int grid
      flush(up, out - w, a, b, i > 0 ? lo[i - 1] : n, i > 0 ? hi[i - 1] : n);
      flush(down, out + w, a, b, i < n - 1 ? lo[i + 1] : n, i < n - 1 ? hi[i + 1] : n);
      changed |= any;
    }
  } while (changed);
  // Copy the Pacman cells back
  for (int i = 0; i < n; ++i)
    for (int j = lo[i]; j <= hi[i]; ++j)
      CELL(i, j) = cells[(i + 1) * w + j + 1];
  free(top - 1);
  free(cells);
}

void find_identity() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      CELL(i, j) = 6;
    }
  }
  stabilize();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      CELL(i, j) = 6 - CELL(i, j);
    }
  }
  stabilize();
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 2) {
    fprintf(stderr, "Usage: %s N\n", argv[0]);
    return EXIT_FAILURE;
  }
  char *end;
  uintmax_t parsed = strtoumax(argv[1], &end, 10);
  if (errno || argv[1][0] < '0' || argv[1][0] > '9' || *end || !parsed || parsed > INT32_MAX)
    die("grid size must be a positive int");
  N = (int)parsed;

  // Border starts empty
  grid = (int *)calloc((size_t)(N + 2) * (N + 2), sizeof(int));
  find_ranges();

  find_identity();

  render("identity.png");
  free(grid);
  free(lo);
  free(hi);
  return EXIT_SUCCESS;
}

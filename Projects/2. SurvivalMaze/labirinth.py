import numpy as np
from random import randrange, choice
from numpy.random import shuffle


def find_neighbors(height, width, r, c, grid, is_wall=False):
    ns = []

    if r > 1 and grid[r - 2][c] == is_wall:
        ns.append((r - 2, c))
    if r < height - 2 and grid[r + 2][c] == is_wall:
        ns.append((r + 2, c))
    if c > 1 and grid[r][c - 2] == is_wall:
        ns.append((r, c - 2))
    if c < width - 2 and grid[r][c + 2] == is_wall:
        ns.append((r, c + 2))

    shuffle(ns)
    return ns


def generate_backtracking(H, W):
    height = 2 * H + 1
    width = 2 * W + 1
    grid = np.empty((height, width), dtype=int)
    grid.fill(1)

    crow = randrange(1, height, 2)
    ccol = randrange(1, width, 2)
    # crow = H
    # ccol = 0
    track = [(crow, ccol)]
    grid[crow][ccol] = 0

    while track:
        (crow, ccol) = track[-1]
        print(crow, ccol)
        neighbors = find_neighbors(height, width, crow, ccol, grid, True)

        if len(neighbors) == 0:
            track = track[:-1]
        else:
            nrow, ncol = neighbors[0]
            grid[nrow][ncol] = 0
            grid[(nrow + crow) // 2][(ncol + ccol) // 2] = 0

            track += [(nrow, ncol)]

    return grid


def generate_binary(H, W):
    height = 2 * H + 1
    width = 2 * W + 1
    # create empty grid, with walls
    grid = np.empty((height, width), dtype=int)
    grid.fill(1)

    for row in range(1, height, 2):
        for col in range(1, width, 2):
            grid[row][col] = 0
            neighbor_row, neighbor_col = find_neighbor_binary(height, width, row, col)
            grid[neighbor_row][neighbor_col] = 0

    return grid


def find_neighbor_binary(height, width, current_row, current_col):
    """Find a neighbor in the skewed direction.
    Args:
        current_row (int): row number
        current_col (int): col number
    Returns:
        tuple: position of the randomly-chosen neighbor
    """
    neighbors = []
    skew = [(1, 0), (0, -1)]
    for b_row, b_col in skew:
        neighbor_row = current_row + b_row
        neighbor_col = current_col + b_col
        if neighbor_row > 0 and neighbor_row < (height - 1):
            if neighbor_col > 0 and neighbor_col < (width - 1):
                neighbors.append((neighbor_row, neighbor_col))

    if len(neighbors) == 0:
        return (current_row, current_col)
    else:
        return choice(neighbors)


if __name__ == '__main__':
    height = 6
    width = 6
    mat = generate_backtracking(height, width)
    # mat[height][0] = 0
    # mat[height][2 * width] = 0
    print('\n'.join([''.join(['{:4}'.format(item) for item in row])
                     for row in mat]))
    print("{")
    for i in range(0, 2 * height + 1):
        print("{", end="")
        for j in range(0, 2 * width + 1):
            if (j == 2 * width):
                print(mat[i][j], end="")
            else:
                print(mat[i][j], end=", ")
        print("},")
    print("}")
    print(mat)

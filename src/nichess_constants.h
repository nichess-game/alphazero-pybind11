#pragma once

namespace alphazero::nichess_gs {

constexpr const int WIDTH = 8;
constexpr const int HEIGHT = 8;
constexpr const int NUM_SQUARES = WIDTH * HEIGHT;
constexpr const int NUM_MAX_POSSIBLE_MOVES_FOR_PIECE = 28;
constexpr const int NUM_MOVES = WIDTH * HEIGHT * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + 1; // +1 for MOVE_SKIP
constexpr const int MOVE_SKIP_IDX = WIDTH * HEIGHT * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE; // Last index is reserved for MOVE_SKIP
constexpr const int NUM_PLAYERS = 2;
constexpr const int NUM_SYMMETRIES = 1; // technically it's 2, but only 1 matters
constexpr const std::array<int, 3> BOARD_SHAPE = {2, HEIGHT, WIDTH};
// 10 piece types (exluding NO_PIECE, 5 for P1 and 5 for P2)
// 2 layers per type.
// 2 layers indicating whether it's P1 or P2 turn.
constexpr const std::array<int, 3> CANONICAL_SHAPE = {10 * 2 + 2, HEIGHT, WIDTH};

} // namespace alphazero::nichess_gs

#pragma once

#include "dll_export.h"
#include "game_state.h"
#include "nichess_wrapper.h"
#include "nichess_constants.h"

namespace alphazero::nichess_gs {

auto gameCache = nichess::GameCache();
auto agentCache = nichess_wrapper::AgentCache();

using CanonicalTensor =
    SizedTensor<float, Eigen::Sizes<CANONICAL_SHAPE[0], CANONICAL_SHAPE[1],
                                    CANONICAL_SHAPE[2]>>;

class DLLEXPORT NichessGS : public GameState {
 public:
  NichessGS() {
    gameWrapper = std::make_unique<nichess_wrapper::GameWrapper>(gameCache, agentCache);
  }

  [[nodiscard]] std::unique_ptr<GameState> copy() const noexcept override;
  [[nodiscard]] bool operator==(const GameState& other) const noexcept override;

  void hash(absl::HashState h) const override;

  // Returns the current player. Players must be 0 indexed.
  [[nodiscard]] uint8_t current_player() const noexcept override {
    return gameWrapper->game->currentPlayer;
  };

  // Returns the current turn.
  [[nodiscard]] uint32_t current_turn() const noexcept override {
    return gameWrapper->game->moveNumber;
  }

  // Returns the number of possible moves.
  [[nodiscard]] uint32_t num_moves() const noexcept override { return NUM_MOVES; }

  // Returns the number of players.
  [[nodiscard]] uint8_t num_players() const noexcept override { return NUM_PLAYERS; };

  // Returns a bool for all moves. The value is true if the move is playable
  // from this GameState.
  [[nodiscard]] Vector<uint8_t> valid_moves() const noexcept override;

  // Plays a move, modifying the current GameState.
  void play_move(uint32_t move) override;

  // Returns nullopt if the game isn't over.
  // Returns a one hot encode result of the game.
  // The first num player positions are set to 1 if that player won and 0
  // otherwise. The last position is set to 1 if the game was a draw and 0
  // otherwise.
  [[nodiscard]] std::optional<Vector<float>> scores() const noexcept override;

  // Returns the canonicalized form of the board, ready for feeding to a NN.
  [[nodiscard]] Tensor<float, 3> canonicalized() const noexcept override;

  // Returns the number of symmetries the game has.
  [[nodiscard]] uint8_t num_symmetries() const noexcept override {
    return NUM_SYMMETRIES;
  }

  // Returns an list of all symetrical game states (including the base state).
  [[nodiscard]] std::vector<PlayHistory> symmetries(
      const PlayHistory& base) const noexcept override;

  // Returns a string representation of the game state.
  [[nodiscard]] std::string dump() const noexcept override;

  // Deletes all data that is not necessary for storing as a hash key.
  // This avoids wasting tons of space when caching states.
  void minimize_storage() override {}

  // In Nichess PlayerAction consists of move + ability. Since that would be too many actions to
  // consider, alphazero only predicts the move while the ability is determined in a rule-based way.
  // This method takes the move index (output of alphazero network), finds the ability and converts 
  // both to a standard Nichess PlayerAction:
  // moveSrcIdx.moveDstIdx.abilitySrcIdx.abilityDstIdx
  [[nodiscard]] std::string move_to_player_action(uint32_t move) const;

 private:
  std::unique_ptr<nichess_wrapper::GameWrapper> gameWrapper;
};

}  // namespace alphazero::nichess_gs

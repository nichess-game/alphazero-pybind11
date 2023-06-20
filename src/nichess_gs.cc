#include "nichess_gs.h"

namespace alphazero::nichess_gs {

[[nodiscard]] std::unique_ptr<GameState> NichessGS::copy() const noexcept {
  auto up = std::make_unique<NichessGS>();
  up->gameWrapper = std::make_unique<nichess_wrapper::GameWrapper>(gameWrapper);
  return up;
}

[[nodiscard]] bool NichessGS::operator==(
    const GameState& other) const noexcept {
  const auto* other_cs = dynamic_cast<const NichessGS*>(&other);
  if (other_cs == nullptr) {
    return false;
  }
  for(int i = 0; i < WIDTH*HEIGHT; i++) {
    if(*gameWrapper->game->board[i] != *other_cs->gameWrapper->game->board[i]) {
      return false;
    }
  }
  return (other_cs->gameWrapper->game->currentPlayer == gameWrapper->game->currentPlayer) &&
    (other_cs->gameWrapper->game->moveNumber == gameWrapper->game->moveNumber);
}

void NichessGS::hash(absl::HashState h) const {
  // TODO: combine_contiguous?
  nichess::Piece* p;
  for(int i = 0; i < WIDTH*HEIGHT; i++) {
    p = gameWrapper->game->board[i];
    h = absl::HashState::combine(std::move(h), p->type, p->healthPoints, p->squareIndex);
  }
  absl::HashState::combine(std::move(h), gameWrapper->game->currentPlayer);
}

[[nodiscard]] Vector<uint8_t> NichessGS::valid_moves() const noexcept {
  auto valids = gameWrapper->computeValids();
  return valids;
}

void NichessGS::play_move(uint32_t move) {
  gameWrapper->makeAction(move);
}

[[nodiscard]] std::optional<Vector<float>> NichessGS::scores() const noexcept {
  std::optional<nichess::Player> winner = gameWrapper->game->winner();
  auto scores = SizedVector<float, 3>{};
  scores.setZero();
  if(winner) {
    if(*winner == nichess::PLAYER_1) {
      scores(0) = 1;
    } else {
      scores(1) = 1;
    }
  } else if(gameWrapper->game->moveNumber == 200) {
    // TODO: Not part of the game, but might help with training
    scores(2) = 1;
  } else {
    return std::nullopt;
  }
  return scores;
}

int pieceTypeToCanonicalIndex(nichess::PieceType pt) {
  switch(pt) {
    case P1_KING:
      return 0;
    case P1_MAGE:
      return 1;
    case P1_PAWN:
      return 2;
    case P1_WARRIOR:
      return 3;
    case P1_ASSASSIN:
      return 4;
    case P2_KING:
      return 5;
    case P2_MAGE:
      return 6;
    case P2_PAWN:
      return 7;
    case P2_WARRIOR:
      return 8;
    case P2_ASSASSIN:
      return 9;
    default:
      return 1000;
  }
}

float pieceTypeToMaxHealthPoints(nichess::PieceType pt) {
  switch(pt) {
    case P1_KING:
      return 200;
    case P1_MAGE:
      return 230;
    case P1_PAWN:
      return 300;
    case P1_WARRIOR:
      return 500;
    case P1_ASSASSIN:
      return 110;
    case P2_KING:
      return 200;
    case P2_MAGE:
      return 230;
    case P2_PAWN:
      return 300;
    case P2_WARRIOR:
      return 500;
    case P2_ASSASSIN:
      return 110;
    default:
      return 0;
  }
}

[[nodiscard]] Tensor<float, 3> NichessGS::canonicalized() const noexcept {
  auto out = CanonicalTensor{};
  out.setZero();
  auto currentPlayerIdx = 19 + gameWrapper->game->currentPlayer;
  auto otherPlayerIdx = 19 + 1 - gameWrapper->game->currentPlayer;
  int currentBoardIdx, currentCanonicalIdx;
  for(auto h = 0; h < HEIGHT; h++) {
    for(auto w = 0; w < WIDTH; w++) {
      // indicates whose turn it is
      out(currentPlayerIdx, h, w) = 1; 

      // each piece has 2 layers
      // first (currentCanonicalIdx) indicates whether piece exists there or not
      // second (currentCanonicalIdx + 10) is used for piece's health points
      currentBoardIdx = nichess::coordinatesToBoardIndex(w, h);
      nichess::Piece* currentPiece = gameWrapper->game->board[currentBoardIdx];
      if(currentPiece->type == nichess::PieceType::NO_PIECE) continue;
      currentCanonicalIdx = pieceTypeToCanonicalIndex(currentPiece->type);
      out(currentCanonicalIdx, h, w) = 1;
      float maxHP = pieceTypeToMaxHealthPoints(currentPiece->type);
      out(currentCanonicalIdx + 10, h, w) = currentPiece->healthPoints / maxHP;
    }
  }
  return out;
}

[[nodiscard]] std::vector<PlayHistory> NichessGS::symmetries(
    const PlayHistory& base) const noexcept {
  std::vector<PlayHistory> syms{base};
  return syms;
}

[[nodiscard]] std::string NichessGS::dump() const noexcept {
  return gameWrapper->game->dump();
}


[[nodiscard]] std::string NichessGS::move_to_player_action(uint32_t move) const {
    return gameWrapper->moveToPlayerAction(move);
 
};

}

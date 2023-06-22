#pragma once

#include "shapes.h"
#include "nichess/nichess.hpp"
#include <vector>

using namespace nichess;
using namespace alphazero;

namespace nichess_wrapper {

class AgentCache {
  public:
      std::vector<std::tuple<int,int>> moveIndexToSrcSquareAndDstSquare;
      std::vector<std::vector<int>> srcSquareToDstSquareToMoveIndex;

      AgentCache();
};

class GameWrapper {
  public:
    Game* game;
    AgentCache* agentCache;

    GameWrapper(nichess::GameCache& gameCache, AgentCache& agentCache);
    GameWrapper(nichess::GameCache& gameCache, AgentCache& agentCache, const std::string encodedBoard);
    GameWrapper(const GameWrapper& other);
    GameWrapper(const std::unique_ptr<nichess_wrapper::GameWrapper>& other);
    ~GameWrapper();

    std::vector<PlayerAction> usefulLegalActionsWithoutMoves();
    std::vector<PlayerMove> legalMovesByPiece(Piece* piece) const;
    Vector<uint8_t> computeValids() const;
    void makeAction(uint32_t move);
    std::string moveToPlayerAction(uint32_t move);
};

} // namespace nichess_wrapper

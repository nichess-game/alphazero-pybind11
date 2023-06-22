#include "nichess_wrapper.h"
#include "nichess/nichess.hpp"
#include "nichess/util.hpp"
#include "nichess_constants.h"

using namespace nichess;
using namespace alphazero::nichess_gs;

std::vector<std::tuple<int, int>> generateMoveIndexToSrcSquareAndDstSquare() {
  // -1 because the last index is reserved for MOVE_SKIP which doesn't have source and destination
  std::vector<std::tuple<int, int>> moveIndexToSrcSquareAndDstSquare(NUM_MOVES - 1);
  int newX, newY, srcSquareIndex, dstSquareIndex, moveIndex;
  for(int srcY = 0; srcY < NUM_ROWS; srcY++) {
    for(int srcX = 0; srcX < NUM_COLUMNS; srcX++) {
      srcSquareIndex = coordinatesToBoardIndex(srcX, srcY);
      int n = 0;
      for(int dy = -2; dy < 3; dy++) {
        for(int dx = -2; dx < 3; dx++) {
          if(dx == 0 && dy == 0) {
            continue; 
          }
          newX = srcX + dx;
          newY = srcY + dy;
          // newX or newY might be off board, but it doesn't matter.
          // Move legality should be checked elsewhere.
          dstSquareIndex = coordinatesToBoardIndex(newX, newY);
          moveIndexToSrcSquareAndDstSquare[srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n] = std::tuple<int, int>(srcSquareIndex, dstSquareIndex);
          n++;
        }
      }
      // 4 extra moves:
      newX = srcX + 3;
      newY = srcY + 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        moveIndexToSrcSquareAndDstSquare[srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n] = std::tuple<int, int>(srcSquareIndex, dstSquareIndex);
      }
      n++;

      newX = srcX + 3;
      newY = srcY - 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        moveIndexToSrcSquareAndDstSquare[srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n] = std::tuple<int, int>(srcSquareIndex, dstSquareIndex);
      }
      n++;

      newX = srcX - 3;
      newY = srcY + 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        moveIndexToSrcSquareAndDstSquare[srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n] = std::tuple<int, int>(srcSquareIndex, dstSquareIndex);
      }
      n++;

      newX = srcX - 3;
      newY = srcY - 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        moveIndexToSrcSquareAndDstSquare[srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n] = std::tuple<int, int>(srcSquareIndex, dstSquareIndex);
      }
      n++;
    }
  }
  return moveIndexToSrcSquareAndDstSquare;
}

std::vector<std::vector<int>> generateSrcSquareToDstSquareToMoveIndex() {
  std::vector<std::vector<int>> srcSquareToDstSquareToMoveIndex(NUM_SQUARES);
  int newX, newY, srcSquareIndex, dstSquareIndex, moveIndex;
  for(int srcY = 0; srcY < NUM_ROWS; srcY++) {
    for(int srcX = 0; srcX < NUM_COLUMNS; srcX++) {
      srcSquareIndex = coordinatesToBoardIndex(srcX, srcY);
      std::vector<int> dstSquareToMoveIndex(NUM_SQUARES);
      int n = 0;
      for(int dy = -2; dy < 3; dy++) {
        for(int dx = -2; dx < 3; dx++) {
          newX = srcX + dx;
          newY = srcY + dy;
          if(isOffBoard(newX, newY)) {
            n++;
            continue; 
          }
          if(dx == 0 && dy == 0) continue;

          dstSquareIndex = coordinatesToBoardIndex(newX, newY);
          dstSquareToMoveIndex[dstSquareIndex] = srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n;
          n++;
        }
      }
      // 4 extra moves:
      newX = srcX + 3;
      newY = srcY + 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        dstSquareToMoveIndex[dstSquareIndex] = srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n;
      }
      n++;

      newX = srcX + 3;
      newY = srcY - 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        dstSquareToMoveIndex[dstSquareIndex] = srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n;
      }
      n++;

      newX = srcX - 3;
      newY = srcY + 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        dstSquareToMoveIndex[dstSquareIndex] = srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n;
      }
      n++;

      newX = srcX - 3;
      newY = srcY - 3;
      if(!isOffBoard(newX, newY)) {
        dstSquareIndex = coordinatesToBoardIndex(newX, newY);
        dstSquareToMoveIndex[dstSquareIndex] = srcSquareIndex * NUM_MAX_POSSIBLE_MOVES_FOR_PIECE + n;
      }
      n++;
      
      srcSquareToDstSquareToMoveIndex[srcSquareIndex] = dstSquareToMoveIndex;
    }
  }
  return srcSquareToDstSquareToMoveIndex;
}

nichess_wrapper::AgentCache::AgentCache() {
  moveIndexToSrcSquareAndDstSquare = generateMoveIndexToSrcSquareAndDstSquare();
  srcSquareToDstSquareToMoveIndex = generateSrcSquareToDstSquareToMoveIndex();
}

nichess_wrapper::GameWrapper::GameWrapper(nichess::GameCache& gameCache, AgentCache& agentCache) {
  this->agentCache = &agentCache;
  game = new Game(gameCache);
}

nichess_wrapper::GameWrapper::GameWrapper(nichess::GameCache& gameCache, AgentCache& agentCache, const std::string encodedBoard) {
  this->agentCache = &agentCache;
  game = new Game(gameCache, encodedBoard);
}

nichess_wrapper::GameWrapper::GameWrapper(const nichess_wrapper::GameWrapper& other) {
  this->agentCache = other.agentCache;
  game = new Game(*other.game);
}

nichess_wrapper::GameWrapper::GameWrapper(const std::unique_ptr<nichess_wrapper::GameWrapper>& other) {
  this->agentCache = other->agentCache;
  game = new Game(*other->game);
}

nichess_wrapper::GameWrapper::~GameWrapper() {
  delete game;
}

/*
 * Useful actions are those whose abilities change the game state.
 * For example, warrior attacking an empty square is legal but doesn't change the game state.
 * Returns the vector of PlayerActions, excluding actions where:
 * 1) Move is different from MOVE_SKIP
 * 2) Ability is ABILITY_SKIP
 */
std::vector<PlayerAction> nichess_wrapper::GameWrapper::usefulLegalActionsWithoutMoves() {
  std::vector<PlayerAction> retval;
  // If King is dead, game is over and there are no legal actions
  if(game->playerToPieces[game->currentPlayer][KING_PIECE_INDEX]->healthPoints <= 0) {
    return retval;
  }

  for(int k = 0; k < NUM_STARTING_PIECES; k++) {
    Piece* cp2 = game->playerToPieces[game->currentPlayer][k];
    if(cp2->healthPoints <= 0) continue; // no abilities for dead pieces
    auto legalAbilities = game->gameCache->pieceTypeToSquareIndexToLegalAbilities[cp2->type][cp2->squareIndex];
    for(int l = 0; l < legalAbilities.size(); l++) {
      Piece* destinationSquarePiece = game->board[legalAbilities[l].abilityDstIdx];
      // exclude useless abilities
      switch(cp2->type) {
        // king can only use abilities on enemy pieces
        case P1_KING:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              continue;
            case P1_MAGE:
              continue;
            case P1_PAWN:
              continue;
            case P1_WARRIOR:
              continue;
            case P1_ASSASSIN:
              continue;
            case P2_KING:
              break;
            case P2_MAGE:
              break;
            case P2_PAWN:
              break;
            case P2_WARRIOR:
              break;
            case P2_ASSASSIN:
              break;
            case NO_PIECE:
              continue;
            default:
              break;
          }
        // mage can only use abilities on enemy pieces
        case P1_MAGE:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              continue;
            case P1_MAGE:
              continue;
            case P1_PAWN:
              continue;
            case P1_WARRIOR:
              continue;
            case P1_ASSASSIN:
              continue;
            case P2_KING:
              break;
            case P2_MAGE:
              break;
            case P2_PAWN:
              break;
            case P2_WARRIOR:
              break;
            case P2_ASSASSIN:
              break;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // pawn can use abilities on enemy pieces
        case P1_PAWN:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              continue;
            case P1_MAGE:
              continue;
            case P1_PAWN:
              continue;
            case P1_WARRIOR:
              continue;
            case P1_ASSASSIN:
              continue;
            case P2_KING:
              break;
            case P2_MAGE:
              break;
            case P2_PAWN:
              break;
            case P2_WARRIOR:
              break;
            case P2_ASSASSIN:
              break;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // warrior can only use abilities on enemy pieces
        case P1_WARRIOR:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              continue;
            case P1_MAGE:
              continue;
            case P1_PAWN:
              continue;
            case P1_WARRIOR:
              continue;
            case P1_ASSASSIN:
              continue;
            case P2_KING:
              break;
            case P2_MAGE:
              break;
            case P2_PAWN:
              break;
            case P2_WARRIOR:
              break;
            case P2_ASSASSIN:
              break;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // assassin can only use abilities on enemy pieces
        case P1_ASSASSIN:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              continue;
            case P1_MAGE:
              continue;
            case P1_PAWN:
              continue;
            case P1_WARRIOR:
              continue;
            case P1_ASSASSIN:
              continue;
            case P2_KING:
              break;
            case P2_MAGE:
              break;
            case P2_PAWN:
              break;
            case P2_WARRIOR:
              break;
            case P2_ASSASSIN:
              break;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;

        // king can only use abilities on enemy pieces
        case P2_KING:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              break;
            case P1_MAGE:
              break;
            case P1_PAWN:
              break;
            case P1_WARRIOR:
              break;
            case P1_ASSASSIN:
              break;
            case P2_KING:
              continue;
            case P2_MAGE:
              continue;
            case P2_PAWN:
              continue;
            case P2_WARRIOR:
              continue;
            case P2_ASSASSIN:
              continue;
            case NO_PIECE:
              continue;
            default:
              break;
          }
        // mage can only use abilities on enemy pieces
        case P2_MAGE:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              break;
            case P1_MAGE:
              break;
            case P1_PAWN:
              break;
            case P1_WARRIOR:
              break;
            case P1_ASSASSIN:
              break;
            case P2_KING:
              continue;
            case P2_MAGE:
              continue;
            case P2_PAWN:
              continue;
            case P2_WARRIOR:
              continue;
            case P2_ASSASSIN:
              continue;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // pawn can use abilities on enemy pieces
        case P2_PAWN:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              break;
            case P1_MAGE:
              break;
            case P1_PAWN:
              break;
            case P1_WARRIOR:
              break;
            case P1_ASSASSIN:
              break;
            case P2_KING:
              continue;
            case P2_MAGE:
              continue;
            case P2_PAWN:
              continue;
            case P2_WARRIOR:
              continue;
            case P2_ASSASSIN:
              continue;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // warrior can only use abilities on enemy pieces
        case P2_WARRIOR:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              break;
            case P1_MAGE:
              break;
            case P1_PAWN:
              break;
            case P1_WARRIOR:
              break;
            case P1_ASSASSIN:
              break;
            case P2_KING:
              continue;
            case P2_MAGE:
              continue;
            case P2_PAWN:
              continue;
            case P2_WARRIOR:
              continue;
            case P2_ASSASSIN:
              continue;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        // assassin can only use abilities on enemy pieces
        case P2_ASSASSIN:
          switch(destinationSquarePiece->type) {
            case P1_KING:
              break;
            case P1_MAGE:
              break;
            case P1_PAWN:
              break;
            case P1_WARRIOR:
              break;
            case P1_ASSASSIN:
              break;
            case P2_KING:
              continue;
            case P2_MAGE:
              continue;
            case P2_PAWN:
              continue;
            case P2_WARRIOR:
              continue;
            case P2_ASSASSIN:
              continue;
            case NO_PIECE:
              continue;
            default:
              break;
          }
          break;
        case NO_PIECE:
          break;
        default:
          break;
      }

      PlayerAction p = PlayerAction(MOVE_SKIP, MOVE_SKIP, legalAbilities[l].abilitySrcIdx, legalAbilities[l].abilityDstIdx);
      retval.push_back(p);
    }
  }
  return retval;
}

/*
 * Assumes piece is alive and belongs to the current player.
 */
std::vector<PlayerMove> nichess_wrapper::GameWrapper::legalMovesByPiece(Piece* piece) const {
  std::vector<PlayerMove> retval;
  auto legalMovesOnEmptyBoard = game->gameCache->pieceTypeToSquareIndexToLegalMoves[piece->type][piece->squareIndex];
  for(int i = 0; i < legalMovesOnEmptyBoard.size(); i++) {
    PlayerMove currentMove = legalMovesOnEmptyBoard[i];
    // Is destination square empty?
    if(game->board[currentMove.moveDstIdx]->type != NO_PIECE) continue;
    // Is p1 pawn trying to jump over another piece?
    if(piece->type == P1_PAWN &&
        piece->squareIndex - currentMove.moveDstIdx == -2 * NUM_COLUMNS 
        ) {
      // checks whether square in front of the p1 pawn is empty
      if(game->board[piece->squareIndex + NUM_COLUMNS]->type != NO_PIECE) continue;
    }
    // Is p2 pawn trying to jump over another piece?
    if(piece->type == P2_PAWN &&
        piece->squareIndex - currentMove.moveDstIdx == 2 * NUM_COLUMNS 
        ) {
      // checks whether square in front of the p2 pawn is empty
      if(game->board[piece->squareIndex - NUM_COLUMNS]->type != NO_PIECE) continue;
    }

    retval.push_back(currentMove);
  }
  return retval;
}

Vector<uint8_t> nichess_wrapper::GameWrapper::computeValids() const {
  auto valids = Vector<uint8_t>{NUM_MOVES};
  valids.setZero();
  int currentIndex;
  bool foundLegalMove = false;
  for(Piece* currentPiece : game->playerToPieces[game->currentPlayer]) {
    if(currentPiece->healthPoints <= 0) continue; // dead pieces don't move
    std::vector<PlayerMove> legalMoves = legalMovesByPiece(currentPiece);
    for(PlayerMove move : legalMoves) {
      currentIndex = agentCache->srcSquareToDstSquareToMoveIndex[move.moveSrcIdx][move.moveDstIdx];
      valids[currentIndex] = 1;
      foundLegalMove = true;
    }
  }
  // last value represents MOVE_SKIP, which is always legal.
  // For some reason, alphazero really loves to skip the move, so this will be forbidden unless
  // it's the only possible move.
  // TODO: I don't like this. Try to run without it, maybe the issue was something else?
  if(!foundLegalMove) {
    valids[MOVE_SKIP_IDX] = 1;
  }

  return valids;
}


float pieceTypeToValueMultiplier(PieceType pt) {
  switch(pt) {
    case P1_KING:
      return 1000;
    case P1_MAGE:
      return  10; 
    case P1_PAWN:
      return 1;
    case P1_WARRIOR:
      return 5;
    case P1_ASSASSIN:
      return 15;
    case P2_KING:
      return 1000;
    case P2_MAGE:
      return 10;
    case P2_PAWN:
      return 1;
    case P2_WARRIOR:
      return 5;
    case P2_ASSASSIN:
      return 15;
    case NO_PIECE:
      return 0;
    default:
      return 0;
  }
}

float positionValue(Game* game, Player player) {
  float retval = 0;
  std::vector<Piece*> p1Pieces = game->getAllPiecesByPlayer(PLAYER_1);
  for(Piece* p : p1Pieces) {
    if(p->healthPoints <= 0) {
      retval -= pieceTypeToValueMultiplier(p->type) * 100;
      continue;
    }
    retval += pieceTypeToValueMultiplier(p->type) * p->healthPoints;
  }
  std::vector<Piece*> p2Pieces = game->getAllPiecesByPlayer(PLAYER_2);
  for(Piece* p : p2Pieces) {
    if(p->healthPoints <= 0) {
      retval += pieceTypeToValueMultiplier(p->type) * 100;
      continue;
    }
    retval -= pieceTypeToValueMultiplier(p->type) * p->healthPoints;
  }
  float m = 1;
  if(player == PLAYER_2) m = -1;
  return m * retval;
}

/*
 * Move is decided by the neural net beforehand and ability is decided by trying out every possible ability and
 * selecting greedily based on the value function.
 */
void nichess_wrapper::GameWrapper::makeAction(uint32_t move) {
  int moveSrcIdx, moveDstIdx;
  if(move == MOVE_SKIP_IDX) {
    moveSrcIdx = MOVE_SKIP;
    moveDstIdx = MOVE_SKIP;
  } else {
    std::tuple<int, int> srcAndDst = agentCache->moveIndexToSrcSquareAndDstSquare[move];
    moveSrcIdx = std::get<0>(srcAndDst);
    moveDstIdx = std::get<1>(srcAndDst);
  }
  int abilitySrcIdx = ABILITY_SKIP;
  int abilityDstIdx = ABILITY_SKIP;
  if(moveSrcIdx != MOVE_SKIP) {
    game->makeMove(moveSrcIdx, moveDstIdx);
  }
  // find best ability
  std::vector<PlayerAction> actions = usefulLegalActionsWithoutMoves();
  float bestValue = -std::numeric_limits<float>::max();
  float currentValue;
  Player player = game->currentPlayer;
  for(PlayerAction pa : actions) {
    UndoInfo undoInfo = game->makeAction(MOVE_SKIP, MOVE_SKIP, pa.abilitySrcIdx, pa.abilityDstIdx);
    currentValue = positionValue(game, ~game->currentPlayer);
    if(currentValue > bestValue) {
      bestValue = currentValue;
      abilitySrcIdx = pa.abilitySrcIdx;
      abilityDstIdx = pa.abilityDstIdx;
    }
    game->undoAction(undoInfo);
  }
  if(moveSrcIdx != MOVE_SKIP) {
    game->undoMove(moveSrcIdx, moveDstIdx);
  }
  game->makeAction(moveSrcIdx, moveDstIdx, abilitySrcIdx, abilityDstIdx);
}

/*
 * Move is decided by the neural net beforehand and ability is decided by trying out every possible ability and
 * selecting greedily based on the value function.
 */
std::string nichess_wrapper::GameWrapper::moveToPlayerAction(uint32_t move) {
  int moveSrcIdx, moveDstIdx;
  if(move == MOVE_SKIP_IDX) {
    moveSrcIdx = MOVE_SKIP;
    moveDstIdx = MOVE_SKIP;
  } else {
    std::tuple<int, int> srcAndDst = agentCache->moveIndexToSrcSquareAndDstSquare[move];
    moveSrcIdx = std::get<0>(srcAndDst);
    moveDstIdx = std::get<1>(srcAndDst);
  }
  int abilitySrcIdx = ABILITY_SKIP;
  int abilityDstIdx = ABILITY_SKIP;
  if(moveSrcIdx != MOVE_SKIP) {
    game->makeMove(moveSrcIdx, moveDstIdx);
  }
  // find best ability
  std::vector<PlayerAction> actions = usefulLegalActionsWithoutMoves();
  float bestValue = -std::numeric_limits<float>::max();
  float currentValue;
  Player player = game->currentPlayer;
  for(PlayerAction pa : actions) {
    UndoInfo undoInfo = game->makeAction(MOVE_SKIP, MOVE_SKIP, pa.abilitySrcIdx, pa.abilityDstIdx);
    currentValue = positionValue(game, ~game->currentPlayer);
    if(currentValue > bestValue) {
      bestValue = currentValue;
      abilitySrcIdx = pa.abilitySrcIdx;
      abilityDstIdx = pa.abilityDstIdx;
    }
    game->undoAction(undoInfo);
  }
  if(moveSrcIdx != MOVE_SKIP) {
    game->undoMove(moveSrcIdx, moveDstIdx);
  }
  std::string retval = "";
  retval += std::to_string(moveSrcIdx) + ".";
  retval += std::to_string(moveDstIdx) + ".";
  retval += std::to_string(abilitySrcIdx) + ".";
  retval += std::to_string(abilityDstIdx);
  return retval;
}

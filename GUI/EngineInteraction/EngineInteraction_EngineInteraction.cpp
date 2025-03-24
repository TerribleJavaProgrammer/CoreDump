#include "board/board.h"
#include "board/magic/magicbitboard.h"
#include "board/position.h"
#include "engine-related/engine.h"
#include "move/movegen.h"
#include "EngineInteraction_EngineInteraction.h"

JNIEXPORT jboolean JNICALL Java_EngineInteraction_EngineInteraction_initGame
  (JNIEnv *env, jobject obj, jboolean playerIsWhite) {
    bool playerIsWhiteBool = (playerIsWhite == JNI_TRUE);
    currPosition.initializePosition();
    initializeMagicBitboards();
    if (!playerIsWhiteBool) {
        std::cout << "player not white" << std::endl;
        currPosition.updatePosition(findBestMove(Move::Color::WHITE, 50, 5));
    }
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_EngineInteraction_EngineInteraction_makeMove
  (JNIEnv *env, jobject obj, jint fromSquare, jint toSquare, jboolean playerIsWhite) {
    Move move;
    move.fromSquare = fromSquare;
    move.toSquare = toSquare;
    if (playerIsWhite == JNI_TRUE) {
        move.color = Move::Color::WHITE;
    } else {
        move.color = Move::Color::BLACK;
    }

    // Check if move is legal
    std::vector<Move> legalMoves = generateMoves(currPosition.getPosition(), move.color);
    auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
        [&move](const Move& m) {
            return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
        });

    if (moveIt == legalMoves.end()) {
        return JNI_FALSE;
    }
    else {
        move = *moveIt;
        if (move.isPromotion) {
            move.promotionPiece = Move::PieceType::QUEEN;
        }
        currPosition.updatePosition(move);
        return JNI_TRUE;
    }
}

JNIEXPORT jstring JNICALL Java_EngineInteraction_EngineInteraction_fetchState
  (JNIEnv *env, jobject obj, jboolean playerIsWhite) {
    std::string state = currPosition.displayPosition();
    return env->NewStringUTF(state.c_str());
}

JNIEXPORT jboolean JNICALL Java_EngineInteraction_EngineInteraction_botMove
  (JNIEnv *env, jobject obj, jboolean botIsBlack) {
    Move::Color botColor = Move::Color::BLACK;
    if (botIsBlack == JNI_FALSE) {
        botColor = Move::Color::WHITE;
    }
    currPosition.updatePosition(findBestMove(botColor, 50, 5));
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_EngineInteraction_EngineInteraction_isCheckMate
  (JNIEnv *env, jobject obj) {
    if ((isInCheck(currPosition, Move::Color::BLACK) && (generateMoves(currPosition, Move::Color::BLACK).size() == 0)) ||
        (isInCheck(currPosition, Move::Color::WHITE) && (generateMoves(currPosition, Move::Color::WHITE).size() == 0))) {
            return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL Java_EngineInteraction_EngineInteraction_isStaleMate
  (JNIEnv *env, jobject obj) {
    if ((!isInCheck(currPosition, Move::Color::BLACK) && (generateMoves(currPosition, Move::Color::BLACK).size() == 0)) ||
        (!isInCheck(currPosition, Move::Color::WHITE) && (generateMoves(currPosition, Move::Color::WHITE).size() == 0))) {
            return JNI_TRUE;
    } else {
        return JNI_FALSE;
    }
}
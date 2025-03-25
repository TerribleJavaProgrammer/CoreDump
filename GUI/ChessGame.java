import EngineInteraction.EngineInteraction;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class ChessGame extends JPanel {
    private static final int TILE_SIZE = 80;
    private static final int BOARD_SIZE = 8;
    private EngineInteraction engine;
    private boolean isWhite = true;
    private int selectedSquare = -1;
    private String boardState;
    
    public ChessGame() {
        this.engine = new EngineInteraction();
        engine.initGame(isWhite);
        updateBoard();
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e) {
                int file = 8 - (e.getX() / TILE_SIZE);
                int rank = 7 - (e.getY() / TILE_SIZE);
                int square = rank * 8 + file;
                handleMove(square);
            }
        });
    }
    
    private void handleMove(int square) {
        if (selectedSquare == -1) {
            selectedSquare = square;
        } else {
            int fromSquare = selectedSquare;
            int toSquare = square;
            boolean success = engine.makeMove(64 - fromSquare, 64 - toSquare, !isWhite);
            System.out.println("Move attempted: " + fromSquare + " -> " + toSquare + " | Success: " + success);
            if (success) {
                updateBoard();
                selectedSquare = -1;
                engine.botMove(!isWhite);
                updateBoard();
            } else {
                System.out.println("Invalid move, resetting selection.");
                selectedSquare = -1;
            }
        }
    }
    
    private void updateBoard() {
        boardState = engine.fetchState(isWhite);
        repaint();
        System.out.println("Updated Board State: " + boardState);
    }
    
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        for (int rank = 0; rank < BOARD_SIZE; rank++) {
            for (int file = 0; file < BOARD_SIZE; file++) {
                boolean isLightSquare = (rank + file) % 2 == 0;
                g.setColor(isLightSquare ? Color.LIGHT_GRAY : Color.DARK_GRAY);
                g.fillRect(file * TILE_SIZE, (7 - rank) * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
        
        if (boardState != null) {
            String[] rows = boardState.split("/");
            for (int rank = 0; rank < BOARD_SIZE; rank++) {
                String[] pieces = rows[rank].split(" ");
                for (int file = 0; file < BOARD_SIZE; file++) {
                    String piece = pieces[file];
                    drawPiece(g, piece, file, 7 - rank);
                }
            }
        }
    }
    
    private void drawPiece(Graphics g, String piece, int file, int rank) {
        if (piece.equals(".")) return;
        g.setColor(Color.BLACK);
        g.setFont(new Font("SansSerif", Font.BOLD, 40));
        g.drawString(piece, file * TILE_SIZE + TILE_SIZE / 3, rank * TILE_SIZE + 2 * TILE_SIZE / 3);
    }
    
    public static void main(String[] args) {
        JFrame frame = new JFrame("Chess GUI");
        ChessGame chessPanel = new ChessGame();
        frame.add(chessPanel);
        frame.setSize(TILE_SIZE * BOARD_SIZE, TILE_SIZE * BOARD_SIZE);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import EngineInteraction.EngineInteraction;

public class ChessGame {
    private JFrame frame;
    private JPanel boardPanel;
    private JTextArea gameStateArea;
    private JButton startButton, botMoveButton;
    private EngineInteraction engineInteraction;
    
    private static final int BOARD_SIZE = 8;
    private JButton[][] boardButtons;
    private boolean whiteToMove = true;

    public ChessGame() {
        System.out.println(System.getProperty("java.library.path"));
        engineInteraction = new EngineInteraction();
        initialize();
    }

    private void initialize() {
        // Frame setup
        frame = new JFrame("Chess Game");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(500, 600);
        frame.setLayout(new BorderLayout());

        // Board panel setup
        boardPanel = new JPanel();
        boardPanel.setLayout(new GridLayout(BOARD_SIZE, BOARD_SIZE));
        boardButtons = new JButton[BOARD_SIZE][BOARD_SIZE];
        
        // Initialize board buttons
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                boardButtons[row][col] = new JButton();
                boardButtons[row][col].setPreferredSize(new Dimension(50, 50));
                boardButtons[row][col].setBackground((row + col) % 2 == 0 ? Color.WHITE : Color.BLACK);
                boardButtons[row][col].addActionListener(new BoardButtonListener(row, col));
                boardPanel.add(boardButtons[row][col]);
            }
        }

        // Game state display
        gameStateArea = new JTextArea();
        gameStateArea.setEditable(false);
        gameStateArea.setPreferredSize(new Dimension(400, 100));

        // Buttons for interacting with the game
        startButton = new JButton("Start Game");
        startButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                startGame();
            }
        });

        botMoveButton = new JButton("Bot Move");
        botMoveButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                botMove();
            }
        });

        // Panel for buttons
        JPanel controlPanel = new JPanel();
        controlPanel.setLayout(new FlowLayout());
        controlPanel.add(startButton);
        controlPanel.add(botMoveButton);

        // Add components to frame
        frame.add(boardPanel, BorderLayout.CENTER);
        frame.add(new JScrollPane(gameStateArea), BorderLayout.SOUTH);
        frame.add(controlPanel, BorderLayout.NORTH);

        frame.setVisible(true);
    }

    private void startGame() {
        // Initialize the game
        boolean isWhite = whiteToMove;
        boolean initialized = engineInteraction.initGame(isWhite);
        if (initialized) {
            gameStateArea.setText("Game Started!\n");
            updateBoard();
        } else {
            gameStateArea.setText("Failed to initialize the game.\n");
        }
    }

    private void botMove() {
        boolean botIsBlack = !whiteToMove;
        boolean botMoved = engineInteraction.botMove(botIsBlack);
        if (botMoved) {
            gameStateArea.setText("Bot has made its move.\n");
            updateBoard();
        } else {
            gameStateArea.setText("Bot failed to make a move.\n");
        }
    }

    private void updateBoard() {
        // Fetch and display the current state of the game
        String gameState = engineInteraction.fetchState(whiteToMove);
        gameStateArea.setText(gameState);
    }

    private class BoardButtonListener implements ActionListener {
        private int row, col;

        public BoardButtonListener(int row, int col) {
            this.row = row;
            this.col = col;
        }

        public void actionPerformed(ActionEvent e) {
            // Handle user move (this can be expanded for selecting and moving pieces)
            int fromSquare = row * 8 + col;
            // Prompt the user for the destination square (simple example, can be improved)
            String toSquareInput = JOptionPane.showInputDialog(frame, "Enter destination square (0-63):");
            if (toSquareInput != null) {
                try {
                    int toSquare = Integer.parseInt(toSquareInput);
                    boolean moveValid = engineInteraction.makeMove(fromSquare, toSquare, whiteToMove);
                    if (moveValid) {
                        gameStateArea.setText("Move made: " + fromSquare + " to " + toSquare + "\n");
                        updateBoard();
                        whiteToMove = !whiteToMove; // Toggle turn
                    } else {
                        gameStateArea.setText("Invalid move.\n");
                    }
                } catch (NumberFormatException ex) {
                    gameStateArea.setText("Invalid input. Enter a number.\n");
                }
            }
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                new ChessGame();
            }
        });
    }
}
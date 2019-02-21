import java.util.*;

// Code Submission for Artificial Intelligence Assignment 2
// Andrew Lenz

class ArgumentArrayParser {
    private int _count;
    String[] args;

    ArgumentArrayParser(String[] s){
        _count = 0;
        args = s;
    }

    // Parses arguments into an array of integers, uses public count to continue where last parse left off
    public void ParseInt(int[] list) {
        boolean parseFinished = false;
        boolean initialBracketFound = false;
        int listIndex = 0;
        while (!parseFinished){
            try{
                int arg = Integer.parseInt(args[_count]);
                list[listIndex] = arg;
                listIndex++;
            }
            catch (NumberFormatException e){
                if (args[_count].equals("]")){
                    // Found the end bracket of the start array
                    parseFinished = true;
                }
                else if (args[_count].equals("[") && !initialBracketFound) {
                    // Valid if start arg, switch initialBracketFound so that
                    // any additional "[" will throw an exception
                    initialBracketFound = true;
                }
                else {
                    System.out.println("Invalid argument: " + args[_count]);
                    System.exit(-1);
                }
            }
            _count++;
        }
    }
}

class SearchManager {

    int GlobalStateIDCounter = 0;
    State StartState;
    State EndState;
    int [] EndBoard;

    PriorityQueue<State> OpenList;
    HashMap<Integer, State> ClosedList;
    HashSet<String> VisitedBoards;
    boolean endFound;

    int OpenNodesCount = 0;
    int ClosedNodesCount = 0;

    class State {
        public int StateID;
        public int ParentID;

        int[] Board;
        public int Gn;
        public int Hn;
        private int _fn;

        public int GetSpaceIndex() {
            for (int i = 0; i < Board.length; i++) {
                if (Board[i] == 0) {
                    return i;
                }
            }
            return -1;
        }

        public int GetFn() {
            _fn = Gn + Hn;
            return _fn;
        }

        int Priority;

        // Constructor for State Class
        public State (int parentID, int[] board, int gn, int hn, int priority) {
            StateID = GlobalStateIDCounter++;
            ParentID = parentID;
            Board = board.clone();
            Gn = gn;
            Hn = hn;
            Priority = priority;
        }

        // Print the properties of  the State
        public void Display(int StateCount) {
            System.out.println("State " + StateCount + " - StateID: " + StateID + " ParentID: " + ParentID +
                    " g(n): " + Gn + " h(n): " + Hn + " f(n): " + GetFn());
            System.out.println("Board: ");
            System.out.println(Board[0] + "\t" + Board[1] + "\t" + Board[2] + "\t" + Board[3]);
            System.out.println(Board[4] + "\t" + Board[5] + "\t" + Board[6] + "\t" + Board[7]);
            System.out.println(Board[8] + "\t" + Board[9] + "\t" + Board[10] + "\t" + Board[11]);
            System.out.println(Board[12] + "\t" + Board[13] + "\t" + Board[14] + "\t" + Board[15]);
        }
    }

    // Comparator to use in the Open List
    class PriorityComparator implements Comparator<State> {
        public int compare(State s1, State s2) {
            if (s1.Priority < s2.Priority)
                return -1;
            if (s1.Priority > s2.Priority)
                return 1;
            return 0;
        }
    }

    // Constructor for Search Manager
    public SearchManager(String[] args) {
        int[] startBoard =  new int[16];
        EndBoard = new int[16];
        ArgumentArrayParser p = new ArgumentArrayParser(args);
        p.ParseInt(startBoard);
        p.ParseInt(EndBoard);

        StartState = new State(-1, startBoard, 0, 0, 0);
    }

    // Returns a vector of all Swaps possible from the given index value
    public Vector<Swap> FindSwaps(int index) {
        // Layout of spaces on gameboard
        // 0  1  2  3
        // 4  5  6  7
        // 8  9  10 11
        // 12 13 14 15
        Vector<Swap> possibleSwaps = new Vector<>();

        int indexRow = FindRow(index);
        int indexColumn = FindColumn(index);

        if (indexColumn != 3)
            // Index is not in rightmost column, possible to swap to right
            possibleSwaps.add(new RightSwap(index));

        if (indexColumn != 0)
            // Index is not in leftmost column, possible to swap to left
            possibleSwaps.add(new LeftSwap(index));

        if (indexRow != 0)
            // Index is not in top row, possible to swap up
            possibleSwaps.add(new UpSwap(index));

        if (indexRow != 3)
            // Index is not in bottom row, possible to swap down
            possibleSwaps.add(new DownSwap(index));

        return possibleSwaps;
    }

    // Return a new Board with the specified Swap applied to the passed Board
    public int[] SwapBoard (int[] b, Swap s) {
        int[] tempB = b.clone();
        int tempIntX = tempB[s.X];
        int tempIntY = tempB[s.Y];
        tempB[s.X] = tempIntY;
        tempB[s.Y] = tempIntX;
        return tempB;
    }

    // Performs Heuristic 1 or 2 search given the passed interval
    public void PerformHeuristicSearch() {
        if (ResetSearch())
            // Reset returns true if start and end boards are the same
            return;

        while (!endFound) {
            // Remove highest priority item from queue
            State examinedState = OpenList.remove();

            // Add the board layout of the examined state to VisitedBoards
            VisitedBoards.add(examinedState.Board.toString());
            ClosedList.put(examinedState.StateID, examinedState);
            ClosedNodesCount++;

            // Using the index of the empty space, determine possible swaps
            int spaceIndex = examinedState.GetSpaceIndex();
            Vector<Swap> possibleSwaps = FindSwaps(spaceIndex);

            // For each possible swap, create a new State if the board has not yet been visited
            for (Swap s:possibleSwaps) {
                int[] tempBoard = SwapBoard(examinedState.Board, s);

                if (!VisitedBoards.contains(tempBoard.toString())) {
                    // Set the cost
                    int costOfMove = tempBoard[s.X];

                    int gn = examinedState.Gn + costOfMove;

                    // Perform Heuristic Analysis
                    int hn = Heuristic(tempBoard);

                    /* Create the next State
                        Properties of next State:
                            ParentID is the StateID of the currently examined State
                            Board is the tempBoard generated from the swap above
                            Gn is the examined state's Gn + the cost to perform the swap to tempBoard
                            Hn is result for Heuristic 1 function
                            Priority is examined gn + hn (or f(n))
                    */
                    State nextState = new State(examinedState.StateID, tempBoard, gn, hn, gn + hn);

                    OpenList.add(nextState);
                    OpenNodesCount++;

                    if (CheckEquals(tempBoard, EndBoard)){
                        // Set EndState, search is complete
                        EndState = nextState;
                        endFound = true;
                        break;
                    }

                }
            }
        }

        PrintResults("A* Heuristic");
    }

    // Compares a board to the EndBoard to determine an H(n) value for the board using heuristic 2
    public int Heuristic(int[] board) {
        int estimatedCost = 0;

        for (int val = 0; val < board.length; val++) {

            // Find the index of the value val in the board
            // Also determine which row and column the index is at
            int bIndex = FindVal(val, board);
            int bIndexCol = FindColumn(bIndex);
            int bIndexRow = FindRow(bIndex);

            // Find the index of the value val in the EndBoard
            // Also determine which row and column the index is at
            int endIndex = FindVal(val, EndBoard);
            int eIndexCol = FindColumn(endIndex);
            int eIndexRow = FindRow(endIndex);

            // Distance is the sum of the differences for columns and rows
            int distance = Math.abs(bIndexCol - eIndexCol) + Math.abs(bIndexRow - eIndexRow);

            distance *= val;

            estimatedCost += distance;
        }
        return estimatedCost;
    }

    // Find the passed value in the passed board and return that index number
    public int FindVal(int val, int [] board) {
        for (int i = 0; i < board.length; i++) {
            if (board[i] == val)
                return i;
        }
        return -1;
    }

    // Find the row that the passed index value is in
    public int FindRow(int index) {
        return Math.floorDiv(index, 4);
    }

    // Find the column that the passed index value is in
    public int FindColumn (int index) {
        return index % 4;
    }

    // Resets all lists and values to start a new search
    public boolean ResetSearch() {
        endFound = false;
        EndState = null;
        OpenList = new PriorityQueue<>(11, new PriorityComparator());
        OpenList.add(StartState);
        OpenNodesCount = 1;
        ClosedNodesCount = 0;
        ClosedList = new HashMap<>();
        VisitedBoards = new HashSet<>();
        GlobalStateIDCounter = 1;

        if (CheckEquals(StartState.Board, EndBoard)) {
            EndState = StartState;
            System.out.println("Start State and End State are identical.");
            return true;
        }
        return false;
    }

    // Returns a true if arrays are equal, false if arrays are unequal
    boolean CheckEquals(int[] v1, int[] v2) {
        boolean result = true;
        for (int i = 0; i < v1.length; i++) {
            if (v1[i] != v2[i]) {
                result = false;
                break;
            }
        }
        return result;
    }

    // Prints results given the specified search type string
    void PrintResults (String SearchType) {
        Stack<State> path =  new Stack<>();
        path.push(EndState);

        State prevState = EndState;

        System.out.println("Results for " + SearchType + " Search: ");

        boolean startFound = false;

        // Look through closed list to find the path looking for the state ID of the previous state's Parent
        // Push Parents onto a Stack
        while (!startFound) {
            State tempState = ClosedList.get(prevState.ParentID);
            path.push(tempState);
            if (tempState.ParentID == -1){
                startFound = true;
            }
            prevState = tempState;
        }

        int stateCount = 0;
        int numberOfMoves = path.size() - 1;

        // Pop states off the stack and Display each state in order
        // Use of the stack allows the path to be printed from Start state to End state
        while(!path.empty()){
            State tempState = path.pop();
            tempState.Display(stateCount);
            System.out.print("\n");
            stateCount++;
        }

        // Print additional info about the results of the search
        System.out.println("Number Of Moves: " + numberOfMoves + " Count of Nodes Added to Open List: " + OpenNodesCount +
                " Count of Nodes Added to Closed List: " + ClosedNodesCount +"\n\n");
    }
}

// Abstract Swap class specifies two locations whose values will be swapped
abstract class Swap {
    int X;
    int Y;
}

// Extension of Swap class to move the passed index to the right
class RightSwap extends Swap {
    public RightSwap(int index) {
        X = index;
        Y = index + 1;
    }
}

// Extension of Swap class to move the passed index to the left
class LeftSwap extends Swap {
    public LeftSwap (int index) {
        X = index;
        Y = index - 1;
    }
}

// Extension of Swap class to move the passed index up
class UpSwap extends Swap {
    public UpSwap (int index) {
        X = index;
        Y = index - 4;
    }
}

// Extension of Swap class to move the passed index down
class DownSwap extends Swap {
    public DownSwap (int index) {
        X = index;
        Y = index + 4;
    }
}

public class APLSourceCodeFile {
    public static void main(String[] args){
        SearchManager manager = new SearchManager(args);
        manager.PerformHeuristicSearch();
    }
}

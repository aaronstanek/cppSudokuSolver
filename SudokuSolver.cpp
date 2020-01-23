#include <fstream>

using namespace std;

void dumpError(const char * message) {
    long messageLength = 0;
    while (message[messageLength] != 0) {
        messageLength++;
    }
    try {
        fstream outfile;
        outfile.open("output.txt", fstream::out | fstream::trunc | fstream::binary);
        outfile.seekp(0,ios_base::beg);
        outfile.write(message,messageLength);
        outfile.close();
        exit(0);
    }
    catch (...) {
        exit(1);
    }
}

struct DataPair {
    char v; // 0-9
    char t; // 0-1
};

void load(DataPair * board) {
    try {
        fstream infile;
        infile.open("input.txt", fstream::in | fstream::binary);
        infile.seekg(0,ios_base::end);
        long fileSize = infile.tellg();
        if (fileSize < 81) {
            dumpError("Error: Input File Too Small\n");
        }
        if (fileSize > 256) {
            dumpError("Error: Input File Too Large\n");
        }
        char * buffer = new char[fileSize];
        if (buffer == nullptr) {
            dumpError("Error: Allocation Failure\n");
        }
        infile.seekg(0,ios_base::beg);
        infile.read(buffer,fileSize);
        infile.close();
        long boardIndex = 0;
        for (long i = 0; i < fileSize; i++) {
            char c = buffer[i];
            if (c == 32 || c == 9 || c == 13 || c == 10) {
                // whitespace
                continue;
            }
            if (boardIndex >= 81) {
                dumpError("Error: Too Many Values in Input\n");
            }
            if (c >= '0' && c <= '9') {
                board[boardIndex].v = c - '0';
                board[boardIndex].t = 1;
            }
            else {
                board[boardIndex].v = 0;
                board[boardIndex].t = 0;
            }
            boardIndex++;
        }
        if (boardIndex < 81) {
            dumpError("Error: Too Few Values in Input\n");
        }
        delete[] buffer;
    }
    catch (...) {
        dumpError("Error: Unable To Load Input File\n");
    }
}

bool testRow(DataPair * board, long index, char value) {
    // move to the start of the row
    index -= index % 9;
    // setup
    bool seen = false;
    // run
    for (long i = 0; i < 9; i++) {
        if (board[index+i].v == value) {
            if (seen) {
                return false;
            }
            else {
                seen = true;
            }
        }
    }
    return true;
}

bool testCol(DataPair * board, long index, char value) {
    // move to the start of the col
    index %= 9;
    // setup
    bool seen = false;
    // run
    for (long i = 0; i < 9; i++) {
        if (board[index+(9*i)].v == value) {
            if (seen) {
                return false;
            }
            else {
                seen = true;
            }
        }
    }
    return true;
}

bool testBox(DataPair * board, long index, char value) {
    // move to the start of the box
    index -= ((index / 9) % 3) * 9;
    index -= index % 3;
    // setup
    bool seen = false;
    // run
    for (long i = 0; i < 3; i++) {
        for (long j = 0; j < 3; j++) {
            if (board[index+(9*i)+j].v == value) {
                if (seen) {
                    return false;
                }
                else {
                    seen = true;
                }
            }
        }
    }
    return true;
}

bool test(DataPair * board, long index) {
    char value = board[index].v;
    if (!testRow(board,index,value)) {
        return false;
    }
    if (!testCol(board,index,value)) {
        return false;
    }
    if (!testBox(board,index,value)) {
        return false;
    }
    return true;
}

void solve(DataPair * board) {
    long index = 0;
    char mode = 0;
    while (true) {
        switch (mode) {
            case 0:
                if (index >= 81) {
                    return;
                }
                if (board[index].t == 0) {
                    mode = 2;
                }
                else {
                    index++;
                }
                break;
            case 1:
                if (index < 0) {
                    dumpError("Error: Unsolvable\n");
                }
                if (board[index].t == 0) {
                    mode = 2;
                }
                else {
                    index--;
                }
                break;
            default:
                board[index].v++;
                if (board[index].v > 9) {
                    board[index].v = 0;
                    index--;
                    mode = 1;
                    break;
                }
                if (test(board,index)) {
                    index++;
                    mode = 0;
                }
        }
    }
}

void save(DataPair * board) {
    try {
        char * buffer = new char[90];
        if (buffer == nullptr) {
            dumpError("Error: Allocation Failure\n");
        }
        long readHead = 0;
        for (long writeHead = 0; writeHead < 90; writeHead++) {
            if (writeHead % 10 == 9) {
                buffer[writeHead] = '\n';
            }
            else {
                buffer[writeHead] = board[readHead].v + '0';
                readHead++;
            }
        }
        fstream outfile;
        outfile.open("output.txt", fstream::out | fstream::trunc | fstream::binary);
        outfile.seekp(0,ios_base::beg);
        outfile.write(buffer,90);
        outfile.close();
        delete[] buffer;
    }
    catch (...) {
        dumpError("Error: Unable To Save Output File\n");
    }
}

int main() {
    try {
        DataPair * board = new DataPair[81];
        if (board == nullptr) {
            dumpError("Error: Allocation Failure\n");
        }
        load(board);
        solve(board);
        save(board);
        delete[] board;
        return 0;
    }
    catch (...) {
        dumpError("Error: Unknown Error\n");
    }
}
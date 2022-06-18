# debug_utils

This is a library with debug utilities. For now, it only contains a ConsoleHistory class.

### ConsoleHistory class

The purpose of this class is to replicate the behaviour an history for a console or terminal:
when pressing the up key you get a later entry from the history and
when pressing the down key you get a more recent entry.
There is a (non-interactive) console application that shows how to use this class.

This is how you would typically use the history in a window application.
```cpp
// Let's use global variables simply to demonstrate how to use the history.

// Let's use an history that can store 16 at most before it starts overwriting.
dbgutils::ConsoleHistory    g_history(16);// capacity of 16 entries

// Along with the history you have a command line.
std::string     g_cmdLine;

void OnKeyPress(Key key) {
    // The UP and DOWN key presses move an internal iteration pointer in the history.
    // This pointer can be "defined" i.e. pointing inside the history at an existing entry or
    // "undefined" meaning it does not point inside the history.
    switch (key) {
        // Move towards the oldest entries by one position.
        // Multiple presses of the UP key will eventually move the iteration pointer
        // to the oldest entry.
        // Subsequent calls to this function will do nothing, leaving the pointer
        // on the oldest entry.
        case UP:   history.go_to_previous();  break;
        
        // Move towards the most recent entries by one position.
        // Multiple presses of the DOWN key will eventually move the iteration pointer outside the history.
        // In that case we will print the original command line.
        case DOWN: history.go_to_next();      break;
        
        // other key cases...
    }
}

void DrawCmdLine(Point position) {
    // Get the string to print on the command line: it is either
    // the original command line or an entry from the history.
    // To determine which one, the history returns an empty string if
    // the iteration pointer is undefined.
    auto line = history.get();
    if (line.length() == 0) {
        line = g_cmdLine;
    }
    
    DrawString(line, position);
}
```
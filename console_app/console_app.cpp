#include <iostream>
#include <vector>
#include <string>
#include <algorithm>//std::reverse
#include "../debug_utils/ConsoleHistory.h"

// Forward declaration of structs and functions
// along with renaming those ugly, long type names.
struct KeyEvent;
using History = dbgutils::ConsoleHistory;
using EventList = std::vector<KeyEvent>;

EventList generate_input_events();
void process_input_events(History &history, const EventList &events);

History create_and_fill_history()
{
	dbgutils::ConsoleHistory history(3);

	history.push("find *.cpp");
	history.push("less foo.cpp");
	history.push("man algorithm");

	return history;
}

int main()
{
	// Init
	auto history = create_and_fill_history();
	auto events = generate_input_events();

	// Run
	process_input_events(history, events);

	return 0;
}

enum KEYACTION {
	KEYACTION_RELEASE,
	KEYACTION_PRESS
};

struct KeyEvent {
	int				action;
	std::string		key;
};

std::vector<KeyEvent> generate_input_events()
{
	return std::vector<KeyEvent>{ {
		KeyEvent{ KEYACTION_PRESS, "up" },
		KeyEvent{ KEYACTION_PRESS, "up" },
		KeyEvent{ KEYACTION_PRESS, "up" },
		KeyEvent{ KEYACTION_PRESS, "down" },
		KeyEvent{ KEYACTION_PRESS, "down" },
		KeyEvent{ KEYACTION_PRESS, "down" }
	}};
}

std::vector<std::string> history_entries_from_latest_to_most_recent(History &history)
{
	// Get the entries from the most recent one to the latest one.
	std::vector<std::string> entries;
	for (size_t i = 0; i < history.size(); i++) {
		history.go_to_previous();

		entries.push_back(history.get());
	}

	// Reset the history iteration
	while (history.go_to_next()) {
		nullptr;
	}

	// Now from the latest one to the most recent one.
	std::reverse(entries.begin(), entries.end());

	return entries;
}

void print_history(History &history)
{
	auto entries = history_entries_from_latest_to_most_recent(history);

	std::cout << "------------------------------" << std::endl;
	std::cout << "----        HISTORY         --" << std::endl;
	std::cout << "------------------------------" << std::endl;
	for (const auto &e : entries) {
		std::cout << e << std::endl;
	}
	std::cout << "------------------------------" << std::endl;
	std::cout << "------------------------------" << std::endl;
}

void display_prompt(const std::string &s)
{
	std::cout << "> " << s << std::endl;
}

void process_input_events(History &history, const EventList &events)
{
	print_history(history);

	// This is the content of the command line.
	// The program prints it when we "leave" the history at some point by pressing the down key multiple times.
	const std::string cmdLine("gdb ./myprogram");

	display_prompt(cmdLine);

	for (const auto &ev : events) {
		auto lineToPrint = cmdLine;

		// Ignore key release events
		if (ev.action == KEYACTION_RELEASE) {
			continue;
		}

		if (ev.key == "up") {
			auto changed = history.go_to_previous();
			if (changed) {
				lineToPrint = history.get();
			}
		}
		else if (ev.key == "down") {
			auto changed = history.go_to_next();
			if (changed) {
				lineToPrint = history.get();
				if (lineToPrint == "") {
					lineToPrint = cmdLine;
				}
			}
		}

		display_prompt(lineToPrint + " (" + ev.key + " pressed)");
	}
}
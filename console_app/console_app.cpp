/*
	This program shows how to use the dbgutils::ConsoleHistory class.
	
	This program simulates the behaviour of a Linux or Window console when you press
	the up or down key.
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>//std::reverse
#include "../debug_utils/ConsoleHistory.h"


//			FORWARD DECLARATIONS
//
#define IN
#define OUT

struct KeyEvent;

using History	= dbgutils::ConsoleHistory;
using EventList = std::vector<KeyEvent>;

EventList	generate_input_events();
History		create_and_fill_history();
void		run(History &history, const EventList &events);


//			ENTRY POINT
//
int main()
{
	// Init
	auto history = create_and_fill_history();
	auto events = generate_input_events();

	run(history, events);

	return 0;
}


//			IMPLEMENTATION OF FORWARD STRUCTS AND FUNCTIONS
//

enum KEYACTION {
	KEYACTION_RELEASE,
	KEYACTION_PRESS
};

struct KeyEvent {
	int				action;
	std::string		key;
};

History create_and_fill_history()
{
	dbgutils::ConsoleHistory history(3);

	history.push("find *.cpp");
	history.push("less foo.cpp");
	history.push("man algorithm");

	return history;
}

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

void handle_key_press_event(IN const std::string &key, IN History &history, IN OUT std::string *line)
{
	if (key == "up") {
		auto changed = history.go_to_previous();
		if (changed) {
			*line = history.get();
		}
	}
	else if (key == "down") {
		auto changed = history.go_to_next();
		if (changed) {
			auto entry = history.get();
			if (entry != "") {
				*line = entry;
			}
		}
	}
}

std::string to_str(const KeyEvent &ev)
{
	return "("
		+ ev.key
		+ " "
		+ (ev.action == KEYACTION_PRESS ? "pressed" : "released")
		+ ")"
		;
}

void run(History &history, const EventList &events)
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

		handle_key_press_event(IN ev.key, IN history, IN OUT &lineToPrint);

		display_prompt(lineToPrint + " " + to_str(ev));
	}
}
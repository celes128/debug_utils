#include "pch.h"
#include "../debug_utils/ConsoleHistory.h"

TEST(ConsoleHistory, Ctor)
{
	dbgutils::ConsoleHistory history(3);

	auto got = history.capacity();
	auto expected = 3;
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, SizeOfOneAfterOnePush)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");

	auto got = history.size();
	auto expected = 1;
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, SizeOfTwoAfterTwoPush)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");
	history.push("b");

	auto got = history.size();
	auto expected = 2;
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, PushUntilFull)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");
	history.push("b");
	history.push("c");

	{
		auto got = history.size();
		auto expected = 3;
		EXPECT_EQ(got, expected);
	}
	{
		EXPECT_TRUE(history.full());
	}
}

// Utils
static std::vector<std::string> collect(size_t n, dbgutils::ConsoleHistory &history)
{
	std::vector<std::string> entries;

	for (size_t i = 0; i < n; i++) {
		history.go_to_previous();

		entries.push_back(history.get());
	}

	return entries;
}

TEST(ConsoleHistory, PushOneAndGetIt)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");

	auto got = collect(2, history);
	std::vector<std::string> expected(2, "a");
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, PushTwoAndGetThem)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");
	history.push("b");

	auto got = collect(3, history);
	auto expected = std::vector<std::string>{ {"b","a","a"} };
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, PushThreeAndGetThem)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");
	history.push("b");
	history.push("c");

	auto got = collect(4, history);
	auto expected = std::vector<std::string>{ {"c","b","a","a"} };
	EXPECT_EQ(got, expected);
}

TEST(ConsoleHistory, OverwriteOneItem)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");
	history.push("b");
	history.push("c");
	history.push("d");

	auto got = collect(4, history);
	auto expected = std::vector<std::string>{ {"d","c","b","b"} };
	EXPECT_EQ(got, expected);
}

// Utils
enum Action { Previous, Next };

static std::vector<std::string> do_actions_and_collect(std::vector<Action> &actions, dbgutils::ConsoleHistory &history)
{
	std::vector<std::string> entries;

	for (const auto &action : actions) {
		switch (action) {
			case Action::Previous:	history.go_to_previous(); break;
			case Action::Next:		history.go_to_next(); break;
			default: break;
		}

		entries.push_back(history.get());
	}

	return entries;
}

TEST(ConsoleHistory, PushOne_PrevNext)
{
	dbgutils::ConsoleHistory history(3);

	history.push("a");

	std::vector<Action> actions{ Previous, Next };
	auto got = do_actions_and_collect(actions, history);
	auto expected = std::vector<std::string>{ "a","" };
	EXPECT_EQ(got, expected);
}
#include "pch.h"
#include "../debug_utils/EditBox.h"

TEST(EditBox, ContentEmptyStr)
{
	dbgutils::EditBox ed;

	auto got = ed.content();
	auto expected = L"";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, Content)
{
	dbgutils::EditBox ed(L"abc");

	auto got = ed.content();
	auto expected = L"abc";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, InsertChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'ㄱ');

	auto got = ed.content();
	auto expected = L"ㄱ";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharLeftChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharLeftCharRightChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');
	ed.handle_key(VK_RIGHT);
	ed.handle_character(L'c');

	auto got = ed.content();
	auto expected = L"abc";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, Backspace)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'b');
	ed.handle_key(VK_BACK);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"a";
	EXPECT_EQ(got, expected);
}

TEST(EditBox, CharBackspaceCharLeftChar)
{
	dbgutils::EditBox ed;
	ed.handle_character(L'c');
	ed.handle_key(VK_BACK);
	ed.handle_character(L'b');
	ed.handle_key(VK_LEFT);
	ed.handle_character(L'a');

	auto got = ed.content();
	auto expected = L"ab";
	EXPECT_EQ(got, expected);
}

// Utility function for the tests.
static void PositionCaretAt(dbgutils::EditBox &ed, size_t position)
{
	// Move the caret to the beginning of the string.
	ed.handle_key(VK_HOME);

	for (size_t i = 0; i < position; i++) {
		ed.handle_key(VK_RIGHT);
	}
}

TEST(EditBox, CtrlLeft)
{
	dbgutils::EditBox ed(L"ab c  def ");

	struct Test {
		size_t	caretBeforeTheCall;
		size_t	expectedCaret;
	};

	std::vector<Test> tests = {
		{0, 0},
		{1, 0},
		{2, 0},
		{3, 0},
		{4, 3},
		{5, 3},
		{6, 3},
		{7, 6},
		{8, 6},
		{9, 6}
	};

	for (const auto &t : tests) {
		PositionCaretAt(ed, t.caretBeforeTheCall);

		// The user presses Ctrl + Left key.
		ed.handle_key(VK_LEFT, ModKeyState{ true, false });
	
		auto got = ed.caret();
		auto exp = t.expectedCaret;
		EXPECT_EQ(got, exp);
	}
}

TEST(EditBox, CtrlLeftEmptyString)
{
	dbgutils::EditBox ed(L"");

	struct Test {
		size_t	caretBeforeTheCall;
		size_t	expectedCaret;
	};
	
	// The user presses Ctrl + Left key.
	ed.handle_key(VK_LEFT, ModKeyState{ true, false });

	auto got = ed.caret();
	auto exp = 0;
	EXPECT_EQ(got, exp);
}

TEST(EditBox, CtrlLeftCharacterOnlyString)
{
	dbgutils::EditBox ed(L"abcdef");

	struct Test {
		size_t	caretBeforeTheCall;
		size_t	expectedCaret;
	};

	std::vector<Test> tests = {
		/*{0, 0},
		{1, 0},*/
		{6, 0}
	};

	for (const auto &t : tests) {
		PositionCaretAt(ed, t.caretBeforeTheCall);

		// The user presses Ctrl + Left key.
		ed.handle_key(VK_LEFT, ModKeyState{ true, false });

		auto got = ed.caret();
		auto exp = t.expectedCaret;
		EXPECT_EQ(got, exp);
	}
}

TEST(EditBox, CtrlLeftWhitespaceOnlyString)
{
	dbgutils::EditBox ed(L"      ");

	struct Test {
		size_t	caretBeforeTheCall;
		size_t	expectedCaret;
	};

	std::vector<Test> tests = {
		{0, 0},
		{1, 0},
		{6, 0}
	};

	for (const auto &t : tests) {
		PositionCaretAt(ed, t.caretBeforeTheCall);

		// The user presses Ctrl + Left key.
		ed.handle_key(VK_LEFT, ModKeyState{ true, false });

		auto got = ed.caret();
		auto exp = t.expectedCaret;
		EXPECT_EQ(got, exp);
	}
}

TEST(EditBox, CtrlRight)
{
	dbgutils::EditBox ed(L"ab c  def ");

	struct Test {
		size_t	caretBeforeTheCall;
		size_t	expectedCaret;
	};

	std::vector<Test> tests = {
		{0, 3},
		{1, 3},
		{2, 3},
		{3, 6},
		{4, 6},
		{5, 6},
		{6, 10},
		{7, 10},
		{8, 10},
		{9, 10},
		{10, 10}
	};

	for (const auto &t : tests) {
		PositionCaretAt(ed, t.caretBeforeTheCall);

		// The user presses Ctrl + Right keys.
		ed.handle_key(VK_RIGHT, ModKeyState{ true, false });

		auto got = ed.caret();
		auto exp = t.expectedCaret;
		EXPECT_EQ(got, exp);
	}
}

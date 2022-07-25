#pragma once

// A dummy command that prints its arguments.
class CommandEcho : public dbgutils::ICommand {
public:
	CommandEcho()
		: dbgutils::ICommand(L"echo")
	{}

	~CommandEcho() = default;

	std::wstring execute(const dbgutils::CmdArgs &args) override
	{
		return wstr_concat(args, L" ");
	}
};

static const std::wstring kLoremIpsumText = 
L"Korean and the closely related Jeju language form the compact Koreanic language family. A relation to the Japonic languages is debated but currently not accepted by most linguists. Another theory is the Altaic Theory Mopak Datu., but it is either discredited or fringe.\n"
L"Homer Hulbert claimed the Korean language was Ural - Altaic in his book The History of Korea(1905).The classification of Korean as Altaic was introduced by Gustaf John Ramstedt(1928), but even within the debunked Altaic hypothesis, the position of Korean relative to Japonic is unclear.A possible Korean�Japonic grouping within Altaic has been discussed by Samuel Martin, Roy Andrew Miller and Sergei Starostin. Others, notably Vovin, interpret the affinities between Korean and Japanese as an effect caused by geographic proximity, i.e.a sprachbund.\n"
L"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue. Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede. Praesent blandit odio eu enim. Pellentesque sed dui ut augue blandit sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aliquam nibh. Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit.\nUt velit mauris, egestas sed, gravida nec, ornare ut, mi. Aenean ut orci vel massa suscipit pulvinar. Nulla sollicitudin. Fusce varius, ligula non tempus aliquam, nunc turpis ullamcorper nibh, in tempus sapien eros vitae ligula. Pellentesque rhoncus nunc et augue. Integer id felis. Curabitur aliquet pellentesque diam. Integer quis metus vitae elit lobortis egestas. Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Morbi vel erat non mauris convallis vehicula. Nulla et sapien. Integer tortor tellus, aliquam faucibus, convallis id, congue eu, quam. Mauris ullamcorper felis vitae erat. Proin feugiat, augue non elementum posuere, metus purus iaculis lectus, et tristique ligula justo vitae magna.\nAliquam convallis sollicitudin purus. Praesent aliquam, enim at fermentum mollis, ligula massa adipiscing nisl, ac euismod nibh nisl eu lectus. Fusce vulputate sem at sapien. Vivamus leo. Aliquam euismod libero eu enim. Nulla nec felis sed leo placerat imperdiet. Aenean suscipit nulla in justo. Suspendisse cursus rutrum augue. Nulla tincidunt tincidunt mi. Curabitur iaculis, lorem vel rhoncus faucibus, felis magna fermentum augue, et ultricies lacus lorem varius purus. Curabitur eu amet.";

// A dummy command that prints a long text so  that I can test the scrolling.
class CommandLoremIpsum : public dbgutils::ICommand {
public:
	CommandLoremIpsum()
		: dbgutils::ICommand(L"loremipsum", L"lor")
	{}

	~CommandLoremIpsum() = default;

	std::wstring execute(const dbgutils::CmdArgs &args) override
	{
		return kLoremIpsumText;
	}
};

// A command that lists all the console commands installed in the interpreter.
class CommandListCommands : public dbgutils::ICommand {
public:
	CommandListCommands(const dbgutils::Interpreter *interpreter = nullptr)
		: dbgutils::ICommand(L"listcmds", L"lc")
		, m_interpreter(interpreter)
	{}

	~CommandListCommands() = default;

	std::wstring execute(const dbgutils::CmdArgs &args) override
	{
		if (!m_interpreter) {
			return L"";
		}

		const auto &commands = m_interpreter->GetCommands();
		std::wstring out;

		for (size_t i = 0; i < commands.size(); i++) {
			const auto &cmd = commands[i];

			out += cmd->Name();

			if (cmd->Alias().length() >= 1) {
				out += L" @" + cmd->Alias();
			}

			if (i + 1 < commands.size()) {
				out += L"\n";
			}
		}

		return out;
	}

private:
	const dbgutils::Interpreter *m_interpreter;
};


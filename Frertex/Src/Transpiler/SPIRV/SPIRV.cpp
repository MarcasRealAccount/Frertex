#include "Frertex/Transpilers/SPIRV/SPIRV.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	State::State(Message::TranspilerMessageHandler messageHandler)
	    : m_MessageHandler(messageHandler) {}

	Binary State::transpile(Utils::CopyMovable<FIL::Binary>&& fil)
	{
		PROFILE_FUNC;

		Binary spirv;

		return spirv;
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startInstruction, std::size_t endInstruction, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_MessageHandler(m_FIL, { type, startInstruction, endInstruction + 1, message.get() });
	}
} // namespace Frertex::Transpilers::SPIRV
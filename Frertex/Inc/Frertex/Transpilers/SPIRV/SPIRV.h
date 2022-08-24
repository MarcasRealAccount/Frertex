#pragma once

#include "Binary.h"
#include "Frertex/FIL/FIL.h"
#include "Frertex/Message.h"
#include "Frertex/Utils/Utils.h"

namespace Frertex::Transpilers::SPIRV
{
	class State
	{
	public:
		State(Message::TranspilerMessageHandler messageHandler);

		Binary transpile(Utils::CopyMovable<FIL::Binary>&& fil);

		auto errored() const { return m_Errored; }

	private:
		void reportMessage(Message::EMessageType type, std::size_t startInstruction, std::size_t endInstruction, Utils::CopyMovable<std::string>&& message);
		void reportWarning(std::size_t startInstruction, std::size_t endInstruction, Utils::CopyMovable<std::string>&& message) { reportMessage(Message::EMessageType::Warning, startInstruction, endInstruction, message.get()); }
		void reportError(std::size_t startInstruction, std::size_t endInstruction, Utils::CopyMovable<std::string>&& message)
		{
			m_Errored = true;
			reportMessage(Message::EMessageType::Error, startInstruction, endInstruction, message.get());
		}

	private:
		Message::TranspilerMessageHandler m_MessageHandler;
		bool                              m_Errored = false;

		FIL::Binary m_FIL;
	};
} // namespace Frertex::Transpilers::SPIRV
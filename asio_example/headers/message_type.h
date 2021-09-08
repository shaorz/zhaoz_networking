#pragma once

#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H
enum class MessageType : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	ServerMessage,
	MessageAll,
};

#endif // !MESSAGE_TYPE_H



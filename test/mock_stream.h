#pragma once

#include <unistd.h>
#include "libp2p/net/stream.h"

struct StreamMessage* mock_message = NULL;
int mock_message_position = 0;

void mock_stream_free(struct Stream* stream);

int mock_stream_close(struct Stream* stream) {
	if (stream == NULL)
		return 0;
	struct ConnectionContext* ctx = (struct ConnectionContext*)stream->stream_context;
	mock_stream_free(stream);
	return 1;
}

int mock_stream_peek(void* context) {
	if (mock_message == NULL)
		return 0;
	return mock_message->data_size;
}

int mock_stream_read(void* context, struct StreamMessage** msg, int timeout_secs) {
	*msg = libp2p_stream_message_copy(mock_message);
	return 1;
}

int mock_stream_read_raw(void* context, uint8_t* buffer, int buffer_size, int timeout_secs) {
	if (mock_message == NULL)
		return 0;
	int min = buffer_size > mock_message->data_size - mock_message_position ? mock_message->data_size - mock_message_position : buffer_size;
	memcpy(buffer, mock_message->data, min);
	mock_message_position += min;
	return min;
}

int mock_stream_write(void* context, struct StreamMessage* msg) {
	return msg->data_size;
}

struct Stream* mock_stream_new() {
	struct Stream* out = libp2p_stream_new();
	if (out != NULL) {
		out->close = mock_stream_close;
		out->peek = mock_stream_peek;
		out->read = mock_stream_read;
		out->read_raw = mock_stream_read_raw;
		out->write = mock_stream_write;
		struct ConnectionContext* ctx = malloc(sizeof(struct ConnectionContext));
		ctx->session_context = (struct SessionContext*)malloc(sizeof(struct SessionContext));
		ctx->session_context->default_stream = out;
		out->stream_context = ctx;
	}
	return out;
}

void mock_stream_free(struct Stream* stream) {
	if (stream == NULL)
		return;
	if (stream->stream_context != NULL) {
		struct ConnectionContext* ctx = (struct ConnectionContext*)stream->stream_context;
		// this will close the session, which will be a loop, so don't
		//libp2p_session_context_free(ctx->session_context);
		free(ctx->session_context);
		free(stream->stream_context);
	}
	free(stream);
}

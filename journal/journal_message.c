#include "ipfs/journal/journal_message.h"
#include "ipfs/journal/journal_entry.h"
#include "libp2p/utils/logger.h"
#include "libp2p/protobuf.h"

struct JournalMessage* ipfs_journal_message_new() {
	struct JournalMessage *message = (struct JournalMessage*) malloc(sizeof(struct JournalMessage));
	if (message != NULL) {
		message->current_epoch = 0;
		message->end_epoch = 0;
		message->start_epoch = 0;
		message->journal_entries = libp2p_utils_vector_new(1);
	}
	return message;
}

int ipfs_journal_message_free(struct JournalMessage* message) {
	if (message != NULL) {
		if (message->journal_entries != NULL) {
			for(int i = 0; i < message->journal_entries->total; i++) {
				struct JournalEntry* entry = (struct JournalEntry*) libp2p_utils_vector_get(message->journal_entries, i);
				ipfs_journal_entry_free(entry);
			}
			libp2p_utils_vector_free(message->journal_entries);
			message->journal_entries = NULL;
		}
		free(message);
	}
	return 1;
}

/**
 * Determine the maximum size of a protobuf'd JournalMessage
 * @param message the JournalMessage
 * @returns the maximum size of this message in bytes if it were protobuf'd
 */
int ipfs_journal_message_encode_size(struct JournalMessage* message) {
	// 3 epochs
	int sz = 33;
	// journal entries
	for (int i = 0; i < message->journal_entries->total; i++) {
		struct JournalEntry* entry = (struct JournalEntry*) libp2p_utils_vector_get(message->journal_entries, i);
		sz += ipfs_journal_entry_encode_size(entry);
	}
	return sz;
}

/***
 * Protobuf the journal message
 * @param message the JournalMessage to protobuf
 * @param buffer where to place the results
 * @param max_buffer_size the amount of memory allocated for the buffer
 * @param bytes_used the amount of the buffer used
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_journal_message_encode(struct JournalMessage* message, uint8_t *buffer, size_t max_buffer_size, size_t *bytes_written) {
	/*
	message JournalMessage {
		int32 current_epoch = 1;
		int32 start_epoch = 2;
		int32 end_epoch = 3;
		repeated JournalEntry journal_entries = 4;
	}
	*/
	// sanity checks
	if (buffer == NULL)
		return 0;
	if (max_buffer_size <= 0)
		return 0;
	*bytes_written = 0;
	size_t bytes_used;
	// current_epoch
	if (!protobuf_encode_varint(1, WIRETYPE_VARINT, message->current_epoch, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	// start_epoch
	if (!protobuf_encode_varint(2, WIRETYPE_VARINT, message->start_epoch, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	// end_epoch
	if (!protobuf_encode_varint(3, WIRETYPE_VARINT, message->end_epoch, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	// journal_entries
	for (int i = 0; i < message->journal_entries->total; i++) {
		struct JournalEntry* entry = (struct JournalEntry*) libp2p_utils_vector_get(message->journal_entries, i);
		// encode the journal entry
		size_t temp_size = ipfs_journal_entry_encode_size(entry);
		uint8_t temp[temp_size];
		if (!ipfs_journal_entry_encode(entry, &temp[0], temp_size, &temp_size))
			return 0;
		if (!protobuf_encode_length_delimited(4, WIRETYPE_LENGTH_DELIMITED, (char*)&temp[0], temp_size, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used)) {
			return 0;
		}
		*bytes_written += bytes_used;
	}
	return 1;
}

/***
 * Turn a protobuf'd JournalMessage and turn it into a real JournalMessage
 * @param incoming the incoming bytes
 * @param incoming_size the size of the incoming buffer
 * @param results where to put the new JournalMessage
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_journal_message_decode(const uint8_t *incoming, size_t incoming_size, struct JournalMessage **out) {
	size_t pos = 0;
	int retVal = 0, got_something = 0;;

	if ( (*out = ipfs_journal_message_new()) == NULL)
		goto exit;

	while(pos < incoming_size) {
		size_t bytes_read = 0;
		int field_no;
		enum WireType field_type;
		if (protobuf_decode_field_and_type(&incoming[pos], incoming_size, &field_no, &field_type, &bytes_read) == 0) {
			goto exit;
		}
		if (field_no < 1 || field_no > 5) {
			libp2p_logger_error("journal_message", "Invalid character in journal_message protobuf at position %lu. Value: %02x\n", pos, incoming[pos]);
		}
		pos += bytes_read;
		switch(field_no) {
			case (1): // current_epoch
				if (protobuf_decode_varint(&incoming[pos], incoming_size - pos, &(*out)->current_epoch, &bytes_read) == 0)
					goto exit;
				pos += bytes_read;
				got_something = 1;
				break;
			case (2): // start_epoch
				if (protobuf_decode_varint(&incoming[pos], incoming_size - pos, &(*out)->start_epoch, &bytes_read) == 0)
					goto exit;
				pos += bytes_read;
				got_something = 1;
				break;
			case (3): // end_epoch
				if (protobuf_decode_varint(&incoming[pos], incoming_size - pos, &(*out)->end_epoch, &bytes_read) == 0)
					goto exit;
				pos += bytes_read;
				got_something = 1;
				break;
			case (4): { // journal entry
				uint8_t *temp;
				size_t temp_length;
				protobuf_decode_length_delimited(&incoming[pos], incoming_size - pos, (char**)&temp, &temp_length, &bytes_read);
				pos += bytes_read;
				struct JournalEntry* entry = NULL;
				if (ipfs_journal_entry_decode(&temp[0], temp_length, &entry)) {
					libp2p_utils_vector_add((*out)->journal_entries, (void*)entry);
					free(temp);
				} else {
					free(temp);
					goto exit;
				}
				got_something = 1;
				break;
			}
		}
	}

	retVal = got_something;

exit:
	if (retVal == 0) {
		ipfs_journal_message_free(*out);
	}
	return retVal;

}

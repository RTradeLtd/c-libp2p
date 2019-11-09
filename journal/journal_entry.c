/**
 * A journal entry protobuf
 */
#include "libp2p/utils/logger.h"
#include "ipfs/journal/journal_entry.h"
#include "libp2p/protobuf.h"

struct JournalEntry* ipfs_journal_entry_new() {
	struct JournalEntry* journal_entry = (struct JournalEntry*) malloc(sizeof(struct JournalEntry));
	if (journal_entry != NULL) {
		journal_entry->hash = NULL;
		journal_entry->hash_size = 0;
		journal_entry->pin = 0;
		journal_entry->timestamp = 0;
	}
	return journal_entry;
}

int ipfs_journal_entry_free(struct JournalEntry* entry) {
	if (entry != NULL) {
		if (entry->hash != NULL) {
			free(entry->hash);
			entry->hash = NULL;
			entry->hash_size = 0;
		}
		free(entry);
	}
	return 1;
}

/**
 * Determine the maximum size of a protobuf'd JournalEntry
 */
int ipfs_journal_entry_encode_size(struct JournalEntry* entry) {
	// hash
	int retVal = entry->hash_size;
	// hash size
	retVal += 11;
	// pin
	retVal += 1;
	// timestamp
	retVal += 11;
	return retVal;
}

/***
 * Protobuf the journal entry
 * @param entry the JournalEntry to protobuf
 * @param buffer where to place the results
 * @param max_buffer_size the amount of memory allocated for the buffer
 * @param bytes_used the amount of the buffer used
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_journal_entry_encode(struct JournalEntry* entry, uint8_t *buffer, size_t max_buffer_size, size_t *bytes_written) {
	/*
	message JournalEntry {
		int32 timestamp = 1;
		string hash = 2;
		bool pin = 3;
	}
	*/
	// sanity checks
	if (buffer == NULL)
		return 0;
	if (max_buffer_size <= 0)
		return 0;
	*bytes_written = 0;
	size_t bytes_used;
	// timestamp
	if (!protobuf_encode_varint(1, WIRETYPE_VARINT, entry->timestamp, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	// hash
	if (!protobuf_encode_length_delimited(2, WIRETYPE_LENGTH_DELIMITED, (char*)entry->hash, entry->hash_size, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	// pin
	if (!protobuf_encode_varint(3, WIRETYPE_VARINT, entry->pin, &buffer[*bytes_written], max_buffer_size - *bytes_written, &bytes_used))
		return 0;
	*bytes_written += bytes_used;
	return 1;
}

/***
 * Turn a protobuf'd JournalEntry and turn it into a real JournalEntry
 * @param incoming the incoming bytes
 * @param incoming_size the size of the incoming buffer
 * @param results where to put the new JournalEntry
 * @returns true(1) on success, false(0) otherwise
 */
int ipfs_journal_entry_decode(uint8_t *incoming, size_t incoming_size, struct JournalEntry **out) {
	size_t pos = 0;
	int retVal = 0, got_something = 0;;

	if ( (*out = ipfs_journal_entry_new()) == NULL)
		goto exit;

	while(pos < incoming_size) {
		size_t bytes_read = 0;
		int field_no;
		enum WireType field_type;
		if (protobuf_decode_field_and_type(&incoming[pos], incoming_size, &field_no, &field_type, &bytes_read) == 0) {
			goto exit;
		}
		if (field_no < 1 || field_no > 5) {
			libp2p_logger_error("journal_entry", "Invalid character in journal_entry protobuf at position %lu. Value: %02x\n", pos, incoming[pos]);
		}
		pos += bytes_read;
		switch(field_no) {
			case (1): // timestamp
				if (protobuf_decode_varint(&incoming[pos], incoming_size - pos, &(*out)->timestamp, &bytes_read) == 0)
					goto exit;
				pos += bytes_read;
				got_something = 1;
				break;
			case (2): // hash
				if (protobuf_decode_length_delimited(&incoming[pos], incoming_size - pos, (char**)&(*out)->hash, &(*out)->hash_size, &bytes_read) == 0)
					goto exit;
				pos += bytes_read;
				got_something = 1;
				break;
			case (3): { // pin
				unsigned long long temp;
				if (protobuf_decode_varint(&incoming[pos], incoming_size - pos, &temp, &bytes_read) == 0)
					goto exit;
				(*out)->pin = (temp == 1);
				pos += bytes_read;
				got_something = 1;
				break;
			}
		}
	}

	retVal = got_something;

exit:
	if (retVal == 0) {
		ipfs_journal_entry_free(*out);
	}
	return retVal;
}

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "datastore.h"
#include "identity.h"

struct Config {
	struct Identity identity;
	struct Datastore datastore;
	//struct address* addresses;
	//struct mount* mounts;
	//struct discovery discovery;
	//struct ipns ipns;
	//struct bootstrap* peer_addresses;
	//struct tour tour;
	//struct gateway gateway;
	//struct supernode_routing supernode_client_config;
	//struct api api;
	//struct swarm swarm;
	//struct reprovider reprovider;
};

/**
 * provide the full path of the config file, given the directory. 
 * NOTE: This allocates memory for result. Make sure to clean up after yourself.
 * @param path the path to the config file (without the actual file name)
 * @param result the full filename including the path
 * @returns true(1) on success, false(0) otherwise
 */
int config_get_file_name(char* path, char* result);

/***
 * Returns the path "extension" relative to the configuration root.
 * If an empty string is provided for config_root, the default root
 * is used.
 * @param config_root the path to the root of the configuration
 * @param extension the extension to add to the path
 * @param result the result of config_root with extension appended
 * @param max_len the max length of the result
 * @returns true(1) if everything went okay, false(0) otherwise
 */
int config_path(char* config_root, char* extension, char* result, int max_len);

#endif

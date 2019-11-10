project('c-ipfs', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])
cc = meson.get_compiler('c')
mbedcrypto_dep = cc.find_library('mbedcrypto')
resolv_dep = cc.find_library('resolv')

libp2p_proj = subproject('libp2p')
libp2p_dep = libp2p_proj.get_variable('libp2p_dep')

lmdb_dep = dependency('lmdb')
curl_dep = dependency('libcurl')
thread_dep = dependency('threads')

ipfs_inc = include_directories('include')
subdir('include')

subdir('blocks')
subdir('cid')
subdir('cmd')
subdir('commands')
subdir('core')
subdir('datastore')
subdir('dnslink')
subdir('exchange')
subdir('flatfs')
subdir('importer')
subdir('ipfsaddr')
subdir('journal')
subdir('main')
subdir('merkledag')
subdir('multibase')
subdir('namesys')
subdir('path')
subdir('pin')
subdir('repo')
subdir('routing')
subdir('unixfs')
subdir('util')
ipfs_lib_sources = [blocks_sources, 
                    cid_sources, 
                    cmd_sources, 
                    commands_sources, 
                    core_sources, 
                    datastore_sources, 
                    dnslink_sources, 
                    exchange_sources, 
                    flatfs_sources, 
                    importer_sources, 
                    ipfsaddr_sources, 
                    journal_sources, 
                    merkledag_sources, 
                    multibase_sources, 
                    namesys_sources, 
                    path_sources, 
                    pin_sources, 
                    repo_sources, 
                    routing_sources, 
                    unixfs_sources, 
                    util_sources]

ipfs_lib = static_library('ipfs_lib', ipfs_lib_sources, include_directories: ipfs_inc,
                          dependencies: [libp2p_dep, lmdb_dep, mbedcrypto_dep, curl_dep, resolv_dep, thread_dep])

ipfs_exe = executable('ipfs', main_sources, include_directories: ipfs_inc,
                      link_with: ipfs_lib, install: true)
cmd_ipfs_exe = executable('cmd_ipfs', cmd_ipfs_sources, include_directories: ipfs_inc,
                          link_with: ipfs_lib)


subdir('test')

project('c-libp2p', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)
thread_dep = dependency('threads')
mbedtls_dep = declare_dependency(dependencies: [cc.find_library('mbedcrypto'),
                                                cc.find_library('mbedtls'),
                                                cc.find_library('mbedx509')])

libp2p_inc = include_directories('include')

subdir('include')
subdir('conn')
subdir('crypto')
subdir('db')
subdir('hashmap')
subdir('identify')
subdir('net')
#subdir('nodeio')
subdir('os')
subdir('peer')
subdir('record')
subdir('routing')
subdir('secio')
subdir('swarm')
subdir('utils')
subdir('yamux')

libp2p_sources = [conn_sources,
                  crypto_sources,
                  db_sources,
                  hashmap_sources,
                  identify_sources,
                  net_sources,
#                  nodeio_sources,
                  os_sources,
                  peer_sources,
                  record_sources,
                  routing_sources,
                  secio_sources,
                  swarm_sources,
                  utils_sources,
                  yamux_sources]

# Internally built static libraries
# These could be subprojects, but meson forbids multiple levels of subprojects, so don't use them, they're a trap
subdir('c-multiaddr')
subdir('c-multihash')
subdir('c-protobuf')

libp2p_lib = library('libp2p', libp2p_sources, include_directories: libp2p_inc,
                     dependencies: [multiaddr_dep, multihash_dep, protobuf_dep, mbedtls_dep, thread_dep, m_dep],
                     install: true)
libp2p_dep = declare_dependency(include_directories: libp2p_inc, link_with: libp2p_lib,
                                dependencies: [multiaddr_dep, multihash_dep, protobuf_dep])

subdir('test')


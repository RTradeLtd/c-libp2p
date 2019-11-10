protobuf_inc = include_directories('.')
install_headers('protobuf.h', 'varint.h', subdir: 'libp2p')

sources = ['main.c', 'protobuf.c', 'varint.c']
protobuf_lib = static_library('ipfs_protobuf', sources, install: true)
protobuf_dep = declare_dependency(link_with: protobuf_lib, include_directories: protobuf_inc)

subdir('test')

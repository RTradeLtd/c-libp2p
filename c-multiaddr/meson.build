cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

multiaddr_inc = include_directories('include')

subdir('include')

sources = ['base58.c', 'multiaddr.c', 'protocols.c', 'protoutils.c', 'varhexutils.c', 'varint.c']

lib_multiaddr = static_library('multiaddr', sources, include_directories: multiaddr_inc, dependencies: m_dep, install: true)
testexe = executable('test_multiaddr', 'testing.c', include_directories: multiaddr_inc, link_with: lib_multiaddr)
test('multiaddr tests', testexe)

multiaddr_dep = declare_dependency(include_directories: multiaddr_inc, link_with: lib_multiaddr)

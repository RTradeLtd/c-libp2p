cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

testexe = executable('testit_libp2p', 'testit.c', include_directories: libp2p_inc, link_with: libp2p_lib, dependencies: [m_dep, multihash_dep, multiaddr_dep, mbedtls_dep])
test('libp2p tests', testexe, should_fail: true)

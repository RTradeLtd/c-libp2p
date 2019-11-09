namesys_sources = files( 'base.c', 'dns.c', 'isdomain.c', 'name.c', 'namesys.c', 'pb.c', 'proquint.c', 'publisher.c', 'resolver.c' )

is_domain_test = executable('is_domain_test', 'is_domain_test.c', 'isdomain.c',
                            include_directories: ipfs_inc)
test('namesys-is_domain_test', is_domain_test)
proquint_test = executable('proquint_test', 'proquint_test.c', 'proquint.c', '../path/path.c',
                           include_directories: ipfs_inc)
test('namesys-proquint_test', proquint_test)

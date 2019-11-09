test_sources = files('testit.c', 'test_helper.c')
test_exe = executable('test_ipfs', test_sources, include_directories: ipfs_inc,
                      link_with: ipfs_lib)
test('test_ipfs', test_exe, should_fail: true)

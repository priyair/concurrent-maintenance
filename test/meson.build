gtest_dep = dependency('gtest', main: true, required: get_option('tests'))

handler_sources = [
    '../src/cm_handler.cpp',
    '../src/cm_object.cpp',
    '../src/fru_identifier.cpp',
    '../src/fsi_card_cm_handler.cpp',
    '../src/bmc_cm_handler.cpp',
    '../src/switchboard_cm_handler.cpp',
]

# test-manager
test_manager = executable(
    'test-manager',
    ['test_manager.cpp', '../src/manager.cpp'] + handler_sources,
    include_directories: inc,
    dependencies: [gtest_dep, sdbusplus_dep, phosphor_logging_dep],
)

# CM handler unit tests
test_cm_handler = executable(
    'test-cm-handler',
    ['test_cm_handler.cpp'] + handler_sources,
    include_directories: inc,
    dependencies: [gtest_dep, sdbusplus_dep, phosphor_logging_dep],
)

test('manager', test_manager)
test('cm-handler', test_cm_handler)

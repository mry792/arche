#! /usr/bin/env python3

from conans import CMake, ConanFile, tools


class ArcheConan (ConanFile):
    name = 'arche'
    version = '0.0.1'

    settings = 'os', 'compiler', 'build_type', 'arch'

    options = {
        'shared': [True, False],
        'fPIC': [True, False],
    }
    default_options = {
        'shared': False,
        'fPIC': True,
    }

    generators = (
        'virtualenv',
        'cmake_find_package',
        'CMakeToolchain',
    )

    scm = {
        'type': 'git',
        'url': 'auto',
        'revision': 'auto',
    }

    # As a header-only library, no need to copy sources to the build folder.
    no_copy_source = True

    @property
    def _run_tests (self):
        return tools.get_env('CONAN_RUN_TESTS', default = False)

    def build_requirements (self):
        if self._run_tests:
            self.test_requires('catch2/2.13.9')
            self.test_requires('trompeloeil/42')

    def config_options (self):
        if self.settings.arch == 'avr':
            del self.options.fPIC

    def build (self):
        if self._run_tests:
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            cmake.test()

    def package (self):
        self.copy('*.hpp')

    def package_id (self):
        self.info.header_only()

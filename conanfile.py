#! /usr/bin/env python3

from conans import ConanFile, tools


class ArcheConan (ConanFile):
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

    @property
    def _run_tests (self):
        return tools.get_env('CONAN_RUN_TESTS', default = False)

    def build_requirements (self):
        if self._run_tests:
            self.test_requires('catch2/2.13.7')

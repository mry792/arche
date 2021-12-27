#! /usr/bin/env python3

from conans import ConanFile


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
        'CMakeToolchain',
    )

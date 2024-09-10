#! /usr/bin/env python3

"""Generate autoconf/CMake checks for C++ feature check macros.

Produces feature checks for those features that we can detect based on just
the C++20 feature check macros.

Reads the test libpqxx feature macro names, as well as the C++ feature check
macros that control them, from cxx_features.txt in the source tree.
"""

from argparse import (
    ArgumentParser,
    Namespace,
)
import os.path
from pathlib import Path
from textwrap import dedent
from typing import cast


# Name of the config file listing, one per line, the macros to test and set.
#
# Ignoring whitespace, a line can be:
# * empty, or
# * a comment starting with '#', or
# * a tuple of macro names.
#
# In that last case, the line must consist of a libpqxx feature macro name
# (which the configuration will either define or not define depening on whether
# there is support for the corresponding feature), followed by the name of the
# C++ feature test macro that we will need to check in order to determinue
# whether that feature is present.
CONFIG = 'cxx_features.txt'


def parse_args() -> Namespace:
    """Parse command line."""
    parser = ArgumentParser(description=__doc__)
    parser.add_argument(
        '--source', '-s', metavar='DIR', type=Path, default=Path(),
        help="Location of the source tree.  Defaults to current directory.")
    return parser.parse_args()


def read_config(config: Path) -> list[tuple[str, str]]:
    """Read config file, return as list of tuples.

    Each item is a tuple of a libpqxx feature macro, and the corresponding
    C++ feature check macro.
    """
    contents = config.read_text(encoding='ascii')
    lines = [line.strip() for line in contents.splitlines()]
    return [
        cast(tuple[str, str], tuple(line.split()))
        for line in lines
        if line != '' and not line.startswith('#')
    ]


# Template for a check snippet.
#
# Strictly speaking, this is unnecessarily verbose.  Why does it check first
# whether the macro is defined, and only then what its value is?  Because it's
# just conceivable that a user might have their compiler set to be extremely
# strict when it comes to checking undefined macros in a conditional.  The
# C++ standard says that in a conditional, an undefined macro shows up as if
# its value were zero.  But I can imagine some people wanting the compiler to
# warn them when they evaluate an undefined macro.
TEMPLATE = dedent("""\
    // Feature check for '%(pqxx_macro)s'.
    // Generated by %(script)s.
    #include <version>
    #if !defined(%(cxx_macro)s)
    #  error "No %(pqxx_macro)s: %(cxx_macro)s is not set."
    #endif
    #if !%(cxx_macro)s
    #  error "No %(pqxx_macro)s: %(cxx_macro)s is false."
    #endif

    int main() {}
    """)


def write_check(
    source: Path, pqxx_macro: str, cxx_macro: str, base_params: dict
    ) -> None:
    """Write a piece of C++ for checking a feature macro."""
    params = dict(
        {
            'pqxx_macro': pqxx_macro,
            'cxx_macro': cxx_macro,
        },
        **base_params)

    check = source / 'config-tests' / f'{pqxx_macro}.cxx'
    check.write_text(TEMPLATE % params, encoding='ascii')



def main(args: Namespace) -> None:
    """Main entry point."""
    items = read_config(args.source / 'cxx_features.txt')
    base_params = {
        'script': os.path.basename(__file__),
    }
    for pqxx_macro, cxx_macro in items:
        write_check(args.source, pqxx_macro, cxx_macro, base_params)


if __name__ == '__main__':
    main(parse_args())